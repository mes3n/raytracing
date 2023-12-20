#include "vec3.h"

#include <math.h>

extern inline vec3 vec3_add(const vec3 v, const vec3 u) {
    return (vec3){
        .x = v.x + u.x,
        .y = v.y + u.y,
        .z = v.z + u.z,
    };
}

extern inline vec3 vec3_sub(const vec3 v, const vec3 u) {
    return (vec3){
        .x = v.x - u.x,
        .y = v.y - u.y,
        .z = v.z - u.z,
    };
}

extern inline vec3 vec3_scale(const vec3 v, const double s) {
    return (vec3){
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s,
    };
}

extern inline double vec3_length_squared(const vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

extern inline double vec3_length(const vec3 v) {
    return sqrt(vec3_length_squared(v));
}

extern inline double vec3_dot(const vec3 v, const vec3 u) {
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

extern inline vec3 vec3_cross(const vec3 v, const vec3 u) {
    return (vec3){
        .x = v.y * u.z - v.z * u.y,
        .y = v.z * u.x - v.x * u.z,
        .z = v.x * u.y - v.y * u.x,
    };
}

extern inline vec3 vec3_normal(const vec3 v) {
    return vec3_scale(v, 1.0 / vec3_length(v));
}
