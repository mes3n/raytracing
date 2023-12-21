#ifndef VEC3_H
#define VEC3_H

#include <math.h>

typedef struct {
    double x, y, z;
} Vec3;

static inline Vec3 vec3_from(const double x, const double y, const double z) {
    return (Vec3){.x = x, .y = y, .z = z};
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

#endif // VEC3_H
