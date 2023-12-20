#ifndef VEC3_H
#define VEC3_H

typedef struct {
    double x, y, z;
} vec3;

extern inline vec3 vec3_add(const vec3 v1, const vec3 v2);
extern inline vec3 vec3_sub(const vec3 v1, const vec3 v2);

extern inline vec3 vec3_scale(const vec3 v, const double s);

extern inline double vec3_length_squared(const vec3 v);
extern inline double vec3_length(const vec3 v);

extern inline double vec3_dot(const vec3 v1, const vec3 v2);
extern inline vec3 vec3_cross(const vec3 v1, const vec3 v2);

extern inline vec3 vec3_normal(const vec3 v);

#endif // VEC3_H
