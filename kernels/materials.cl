#pragma once

#include "common.h.cl"
#include "math.cl"

bool lambertian_scatter(const struct Lambertian *lambertian,
                        const HitRecord *hr, uint *state, Ray *ray,
                        float3 *attenuation) {
    float3 direction = hr->normal + random_unit(state);
    if (dot(direction, direction) < 1e-8f) {
        direction = hr->normal;
    }

    *ray = timed_ray_from(hr->point, direction, ray->time);
    *attenuation = lambertian->albedo;
    // *attenuation = lambertian->texture->color(lambertian->texture, hr->u,
    // hr->v, hr->point);

    return true;
}

bool metal_scatter(const struct Metal *metal, const HitRecord *hr, uint *state,
                   Ray *ray, float3 *attenuation) {
    float3 direction = reflect(normalize(ray->direction), hr->normal);

    *ray = timed_ray_from(
        hr->point, direction + random_unit(state) * metal->fuzz, ray->time);
    *attenuation = metal->albedo;

    return dot(ray->direction, hr->normal) > 0.0f;
}

static inline float reflectance(float cosine, float ri) {
    float r0 = (1.0f - ri) / (1.0f + ri);
    r0 *= r0;

    float c = 1.0f - cosine;
    float c2 = c * c;
    return r0 + (1.0f - r0) * c2 * c2 * c;
}

bool dielectric_scatter(const struct Dielectric *dielectric,
                        const HitRecord *hr, uint *state, Ray *ray,
                        float3 *attenuation) {
    float refraction_ratio = hr->front_face ? (1.0f / dielectric->refraction)
                                            : dielectric->refraction;

    float3 unit_direction = normalize(ray->direction);
    float cos_theta = fmin(dot(-unit_direction, hr->normal), 1.0f);
    float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

    float3 direction;
    float3 origin = hr->point;
    if (refraction_ratio * sin_theta > 1.0f ||
        reflectance(cos_theta, refraction_ratio) > random(state)) {
        direction = reflect(unit_direction, hr->normal);
    } else {
        direction = refract(unit_direction, hr->normal, refraction_ratio);
    }
    *ray = timed_ray_from(origin, direction, ray->time);
    *attenuation = 1.0f;
    return true;
}
