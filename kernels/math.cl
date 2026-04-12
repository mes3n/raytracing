#pragma once

#include "common.h.cl"

static inline void set_face_normal(HitRecord *hit_record, const Ray *ray,
                                   const float3 out_n) {
    hit_record->front_face = dot(ray->direction, out_n) < 0;
    hit_record->normal = hit_record->front_face ? out_n : -out_n;
}

static inline float3 ray_at(const Ray *ray, float t) {
    return ray->origin + ray->direction * t;
}

static inline bool surrounds(const Interval *interval, float x) {
    return interval->min < x && x < interval->max;
}

static inline Ray timed_ray_from(const float3 origin, const float3 direction,
                                 const float time) {
    return (Ray){origin, direction, time};
}

static inline float3 reflect(const float3 v, const float3 normal) {
    return v - normal * 2 * dot(v, normal);
}

static inline float3 refract(const float3 uv, const float3 normal,
                             const float etai_over_etat) {
    const float cos_theta = fmin(dot(-uv, normal), 1);
    const float3 r_out_perp = uv + normal * cos_theta * etai_over_etat;
    const float3 r_out_parallel =
        normal * -sqrt(fabs(1 - dot(r_out_perp, r_out_perp)));
    return r_out_perp + r_out_parallel;
}

float random(__global uint *state) {
    *state *= *state * 74779405 + 2891336453;
    uint result = ((*state >> ((*state >> 28) + 4)) ^ *state) * 277803737;
    result = (result >> 22) ^ result;
    return result / 4294967295.0;
}

float random_guassian(__global uint *state) {
    float theta = 2 * 3.1415926 * random(state);
    float rho = sqrt(-2 * log(random(state)));
    return rho * cos(theta);
}

float2 random_in_circle(__global uint *state) {
    float r = sqrt(random(state));
    float v = 2 * 3.1415926 * random(state);
    return (float2)(r * sin(v), r * cos(v));
}

float3 random_unit(__global uint *state) {
    float x = random_guassian(state);
    float y = random_guassian(state);
    float z = random_guassian(state);
    return normalize((float3)(x, y, z));
}
