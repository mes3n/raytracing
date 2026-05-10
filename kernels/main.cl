#include "common.h.cl"

#include "materials.cl"
#include "math.cl"
#include "shapes.cl"

float3 defocus_disk_sample(__constant const Camera *camera, uint *state) {
    float2 p = random_in_circle(state);
    return camera->defocus_disc_u * p.x + camera->defocus_disc_v * p.y +
           camera->origin;
}

Ray get_ray(__constant const Camera *camera, const float x, const float y,
            uint *state) {
    float3 pixel_sample = (float3)(random(state) - 0.5f, random(state) - 0.5f,
                                   random(state) - 0.5f) *
                          5e-3f;
    float3 vp_pixel = camera->viewport.pos_at00 +
                      camera->viewport.u * (x + pixel_sample.x) +
                      camera->viewport.v * (y + pixel_sample.y);
    vp_pixel += pixel_sample;
    float3 ray_origin = camera->defocus_angle < 0
                            ? camera->origin
                            : defocus_disk_sample(camera, state);
    float3 ray_direction = vp_pixel - ray_origin;
    float ray_time = random(state);

    return (Ray){ray_origin, ray_direction, ray_time};
}

__kernel void create_rays(__constant const Camera *camera, const uint seed,
                          __global Ray *rays) {
    const size_t x = get_global_id(0);
    const size_t y = get_global_id(1);
    const size_t width = get_global_size(0);
    const size_t height = get_global_size(1);

    uint state = seed + y * width + x;

    for (int i = 0; i < camera->samples_per_pixel; i++) {
        const Ray ray = get_ray(camera, (float)x / (float)width,
                                (float)y / (float)height, &state);
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

    return (t_far >= t_near) && (t_far > rayt.min) && (t_near < rayt.max);
}

bool bvh_hit(__constant const Bvh *bvhs, __constant const Hittable *shapes,
             const Ray *ray, Interval rayt, HitRecord *hr) {
    uint stack[32];
    int ptr = 0;

    stack[ptr++] = 0;
    bool hit_any = false;
    while (ptr > 0) {
        Bvh bvh = bvhs[stack[--ptr]];
        if (bbox_hit(&bvh.bbox, ray, rayt)) {
            if (bvh.hittable_idx != CL_SIZE_MAX) {
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
                hr->material_idx =
                    shape_hit ? shape.material_idx : hr->material_idx;
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
                         __constant const Material *materials, uint depth,
                         const uint seed, __global float3 *colors) {
    size_t idx = get_global_id(0);
    Ray ray = rays[idx];
    uint state = seed + idx;

    float3 color = 1;
    while (depth-- > 0) {
        HitRecord hr;
        if (bvh_hit(bvhs, shapes, &ray, (Interval){1e-3, INFINITY}, &hr)) {
            Material mat = materials[hr.material_idx];
            float3 attenuation;
            switch (mat.mat_type) {
            case LAMBERTIAN:
                lambertian_scatter(&mat.mat.lambertian, &hr, &state, &ray,
                                   &attenuation);
                break;
            case METAL:
                attenuation = metal_scatter(&mat.mat.metal, &hr, &state, &ray,
                                            &attenuation)
                                  ? attenuation
                                  : 0;
                break;
            case DIELECTRIC:
                dielectric_scatter(&mat.mat.dielectric, &hr, &state, &ray,
                                   &attenuation);
                break;
            }
            color *= attenuation;
        } else {
            color *= (float3)(0.5f, 0.7f, 1.0f);
            break;
        }
    }

    colors[idx] = color;
}

__kernel void average_samples(__global float3 *colors,
                              const uint samples_per_pixel) {
    const size_t idx = get_global_id(0) * samples_per_pixel;

    for (uint i = 1; i < samples_per_pixel; i++) {
        colors[idx] += colors[idx + i];
    }
    colors[idx] /= (float)samples_per_pixel;
}
