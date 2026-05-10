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
    return v - normal * 2.0f * dot(v, normal);
}

static inline float3 refract(const float3 uv, const float3 normal,
                             const float etai_over_etat) {
    const float cos_theta = fmin(dot(-uv, normal), 1.0f);
    const float3 r_out_perp = (uv + normal * cos_theta) * etai_over_etat;
    const float3 r_out_parallel =
        normal * -sqrt(fabs(1.0f - dot(r_out_perp, r_out_perp)));
    return r_out_perp + r_out_parallel;
}

/// 0 < r < 1
float random(uint *state) {
    *state *= *state * 74779405 + 2891336453;
    uint result = ((*state >> ((*state >> 28) + 4)) ^ *state) * 277803737;
    result = (result >> 22) ^ result;
    return result / 4294967295.0f;
}

float random_guassian(uint *state) {
    float theta = 2.0f * M_PI_F * random(state);
    float rho = sqrt(-2.0f * log(random(state)));
    return rho * cos(theta);
}

float2 random_in_circle(uint *state) {
    float r = sqrt(random(state));
    float v = 2.0f * M_PI_F * random(state);
    return (float2)(r * sin(v), r * cos(v));
}

float3 random_unit(uint *state) {
    float x = random_guassian(state);
    float y = random_guassian(state);
    float z = random_guassian(state);
    return normalize((float3)(x, y, z));
}
