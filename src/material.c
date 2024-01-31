#include "material.h"

bool lambertian_scatter(const Lambertian *lambertian, const Ray *ray,
                        const HitRecord *hr, Ray *scattered,
                        Vec3 *attenuation) {
    Vec3 direction = vec3_add(hr->normal, vec3_random_unit());
    if (vec3_length_squared(direction) < 1e-8) {
        direction = hr->normal;
    }

    *scattered = (Ray){.origin = hr->point, direction};
    *attenuation = lambertian->albedo;

    return true;
}

bool metal_scatter(const Metal *metal, const Ray *ray, const HitRecord *hr,
                   Ray *scattered, Vec3 *attenuation) {
    Vec3 direction = vec3_reflect(vec3_normal(ray->direction), hr->normal);

    *scattered =
        (Ray){.origin = hr->point,
              vec3_add(direction, vec3_scale(vec3_random_unit(), metal->fuzz))};
    *attenuation = metal->albedo;

    return vec3_dot(scattered->direction, hr->normal) > 0.0;
}
