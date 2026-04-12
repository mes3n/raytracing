#pragma once

#include "common.h.cl"
#include "math.cl"

bool lambertian_scatter(const struct Lambertian *lambertian,
                        const HitRecord *hr, Ray *ray, float3 *attenuation) {
    float3 direction = hr->normal + random_unit(NULL);
    if (dot(direction, direction) < 1e-8) {
        direction = hr->normal;
    }

    *ray = timed_ray_from(hr->point, direction, ray->time);
    *attenuation = lambertian->albedo;
    // *attenuation = lambertian->texture->color(lambertian->texture, hr->u,
    // hr->v, hr->point);

    return true;
}

bool metal_scatter(const struct Metal *metal, const HitRecord *hr, Ray *ray,
                   float3 *attenuation) {
    float3 direction = reflect(normalize(ray->direction), hr->normal);

    *ray = timed_ray_from(
        hr->point, direction + random_unit(NULL) + metal->fuzz, ray->time);
    *attenuation = metal->albedo;

    return dot(ray->direction, hr->normal) > 0;
}

static inline float reflectance(float cosine, float ri) {
    float r0 = (1 - ri) / (1 + ri);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool dielectric_scatter(const struct Dielectric *dielectric,
                        const HitRecord *hr, Ray *ray, float3 *attenuation) {
    *attenuation = (float3)(1, 1, 1);
    float refraction_ratio =
        hr->front_face ? (1 / dielectric->refraction) : dielectric->refraction;

    float3 unit_direction = normalize(ray->direction);
    float cos_theta = fmin(dot(unit_direction * -1, hr->normal), 1);
    float sin_theta = sqrt(1 - cos_theta * cos_theta);

    float3 direction;
    if (refraction_ratio * sin_theta > 1 ||
        reflectance(cos_theta, refraction_ratio) > random(NULL)) {
        direction = reflect(unit_direction, hr->normal);
    } else {
        direction = refract(unit_direction, hr->normal, refraction_ratio);
    }
    *ray = timed_ray_from(hr->point, direction, ray->time);
    return true;
}
