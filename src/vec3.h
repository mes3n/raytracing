#ifndef VEC3_H
#define VEC3_H

#include <math.h>

#include <stdlib.h>

typedef struct {
    double x, y, z;
} Vec3;

static inline double random_double() {
    return (double)rand() / (RAND_MAX + 1.0);
}

static inline double random_double_range(double min, double max) {
    return min + (max - min) * random_double();
}

static inline Vec3 vec3_zero() { return (Vec3){.x = 0.0, .y = 0.0, .z = 0.0}; }

static inline Vec3 vec3_from(const double x, const double y, const double z) {
    return (Vec3){.x = x, .y = y, .z = z};
}

static inline Vec3 vec3_random() {
    return (Vec3){
        .x = random_double(),
        .y = random_double(),
        .z = random_double(),
    };
}

static inline Vec3 vec3_random_range(double min, double max) {
    return (Vec3){
        .x = random_double_range(min, max),
        .y = random_double_range(min, max),
        .z = random_double_range(min, max),
    };
}

static inline Vec3 vec3_random_unit() {
    double a = random_double() * 2.0 * M_PI;
    double z = random_double() * 2.0 - 1.0;
    double r = sqrt(1.0 - z * z);
    return (Vec3){
        .x = r * cos(a),
        .y = r * sin(a),
        .z = z,
    };
}

static inline Vec3 vec3_add(const Vec3 v, const Vec3 u) {
    return (Vec3){
        .x = v.x + u.x,
        .y = v.y + u.y,
        .z = v.z + u.z,
    };
}

static inline Vec3 vec3_sub(const Vec3 v, const Vec3 u) {
    return (Vec3){
        .x = v.x - u.x,
        .y = v.y - u.y,
        .z = v.z - u.z,
    };
}

static inline Vec3 vec3_scale(const Vec3 v, const double s) {
    return (Vec3){
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s,
    };
}

static inline Vec3 vec3_scale_from_vec3(const Vec3 v, const Vec3 u) {
    return (Vec3){
        .x = v.x * u.x,
        .y = v.y * u.y,
        .z = v.z * u.z,
    };
}

static inline double vec3_length_squared(const Vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline double vec3_length(const Vec3 v) {
    return sqrt(vec3_length_squared(v));
}

static inline double vec3_dot(const Vec3 v, const Vec3 u) {
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

static inline Vec3 vec3_cross(const Vec3 v, const Vec3 u) {
    return (Vec3){
        .x = v.y * u.z - v.z * u.y,
        .y = v.z * u.x - v.x * u.z,
        .z = v.x * u.y - v.y * u.x,
    };
}

static inline Vec3 vec3_normal(const Vec3 v) {
    return vec3_scale(v, 1.0 / vec3_length(v));
}

static inline Vec3 vec3_reflect(const Vec3 v, const Vec3 normal) {
    return vec3_sub(v, vec3_scale(normal, 2.0 * vec3_dot(v, normal)));
}

static inline Vec3 vec3_refract(const Vec3 uv, const Vec3 normal,
                                const double etai_over_etat) {
    const double cos_theta = fmin(vec3_dot(vec3_scale(uv, -1.0), normal), 1.0);
    const Vec3 r_out_perp =
        vec3_scale(vec3_add(uv, vec3_scale(normal, cos_theta)), etai_over_etat);
    const Vec3 r_out_parallel =
        vec3_scale(normal, -sqrt(fabs(1.0 - vec3_length_squared(r_out_perp))));
    return vec3_add(r_out_perp, r_out_parallel);
}

static inline Vec3 vec3_random_on_hemishpere(const Vec3 normal) {
    Vec3 on_unit_sphere = vec3_random_unit();
    if (vec3_dot(on_unit_sphere, normal) < 0.0) {
        return vec3_scale(on_unit_sphere, -1.0);
    }
    return on_unit_sphere;
}

static inline Vec3 vec3_random_in_unit_disk() {
    while (1) {
        Vec3 p = (Vec3){
            .x = random_double(),
            .y = random_double(),
            .z = 0.0,
        };
        if (vec3_length_squared(p) <= 1.0) {
            continue;
        }
        return p;
    }
}

#endif // VEC3_H
