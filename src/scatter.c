#include "scatter.h"
#include "vec3.h"

bool lambertian_scatter(const Ray *ray, const HitRecord *hr, Ray *scattered) {
    // const Vec3 albedo = vec3_from(0.5, 0.5, 0.5);

    Vec3 direction = vec3_add(hr->normal, vec3_random_unit());
    if (vec3_length_squared(direction) < 1e-8) {
        direction = hr->normal;
    }

    *scattered = (Ray){.origin = hr->point, direction};

    return true;
}

bool metal_scatter(const Ray *ray, const HitRecord *hr, Ray *scattered) {
    // const Vec3 albedo = vec3_from(0.5, 0.5, 0.5);

    Vec3 direction = vec3_reflect(vec3_normal(ray->direction), hr->normal);
    *scattered = (Ray){.origin = hr->point, direction};

    return true;
}
