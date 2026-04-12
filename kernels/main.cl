#include "common.h.cl"

#include "materials.cl"
#include "shapes.cl"

float3 defocus_disk_sample(__constant const Camera *camera,
                           __global uint *seed) {
    float2 p = random_in_circle(seed);
    return camera->defocus_disc_u * p.x + camera->defocus_disc_v * p.y +
           camera->origin;
}

Ray get_ray(__constant const Camera *camera, const float x, const float y,
            __global uint *seed) {
    float3 vp_pixel = camera->viewport.pos_at00 +
                      camera->viewport.u * (x + (random(seed) - 0.5f) * 0) +
                      camera->viewport.v * (y + (random(seed) - 0.5f) * 0);
    float3 ray_origin = camera->defocus_angle < 0
                            ? camera->origin
                            : defocus_disk_sample(camera, seed);
    float3 ray_direction = vp_pixel - ray_origin;
    float ray_time = random(seed);

    return (Ray){ray_origin, ray_direction, ray_time};
}

__kernel void create_rays(__constant const Camera *camera, __global uint *seed,
                          __global Ray *rays) {
    const size_t x = get_global_id(0);
    const size_t y = get_global_id(1);
    const size_t width = get_global_size(0);
    const size_t height = get_global_size(1);

    for (int i = 0; i < camera->samples_per_pixel; i++) {
        const Ray ray = get_ray(camera, (float)x / (float)width,
                                (float)y / (float)height, seed);
        const size_t idx = (y * width + x) * camera->samples_per_pixel + i;

        rays[idx] = ray;
    }
}

bool bbox_hit(const Bbox *bbox, const Ray *ray, Interval rayt) {
    const float3 inv_dir = 1.0f / ray->direction;
    float3 t0 = (bbox->min - ray->origin) * inv_dir;
    float3 t1 = (bbox->max - ray->origin) * inv_dir;

    float3 tmin = fmin(t0, t1);
    float3 tmax = fmax(t0, t1);

    float t_near = fmax(fmax(tmin.x, tmin.y), tmin.z);
    float t_far = fmin(fmin(tmax.x, tmax.y), tmax.z);

    return (t_far >= t_near) && (t_far > 0.0f);
}

bool bvh_hit(__constant const Bvh *bvhs, __constant const Hittable *shapes,
             const Ray *ray, Interval rayt, HitRecord *hr) {
    size_t stack[32];
    int ptr = 0;

    stack[ptr++] = 0;
    bool hit_any = false;

    while (ptr > 0) {
        Bvh bvh = bvhs[stack[--ptr]];

        if (bbox_hit(&bvh.bbox, ray, rayt)) {
            if (bvh.hittable_idx != SIZE_MAX) {
                Hittable shape = shapes[bvh.hittable_idx];
                bool shape_hit = false;
                switch (shape.shape_type) {
                case SPHERE:
                    shape_hit = sphere_hit(&shape.shape.sphere, ray, &rayt, hr);
                    break;
                case QUAD:
                    shape_hit = quad_hit(&shape.shape.quad, ray, &rayt, hr);
                    break;
                }
                rayt.max = shape_hit ? hr->t : rayt.max;
                hit_any |= shape_hit;
            } else {
                stack[ptr++] = bvh.left_idx;
                stack[ptr++] = bvh.right_idx;
            }
        }
    }

    return hit_any;
}

__kernel void trace_rays(__constant const Ray *rays, __constant const Bvh *bvhs,
                         __constant const Hittable *shapes,
                         __constant const Material *materials, const uint depth,
                         __global float3 *colors) {
    size_t idx = get_global_id(0);
    Ray ray = rays[idx];

    float3 color = 1;
    for (int i = 0; i < depth; i++) {
        HitRecord hr;
        if (bvh_hit(bvhs, shapes, &ray, (Interval){1e-3, INFINITY}, &hr)) {
            Material mat = materials[hr.material_idx];
            float3 attenuation;
            switch (mat.mat_type) {
            case LAMBERTIAN:
                lambertian_scatter(&mat.mat.lambertian, &hr, &ray,
                                   &attenuation);
                break;
            case METAL:
                metal_scatter(&mat.mat.metal, &hr, &ray, &attenuation);
                // This might need to be zeroed
                break;
            case DIELECTRIC:
                dielectric_scatter(&mat.mat.dielectric, &hr, &ray,
                                   &attenuation);
                break;
            }
            color *= attenuation;
        } else {
            color *= (float3)(0.5, 0.7, 1.0);
            break;
        }
    }

    colors[idx] = color;
}
