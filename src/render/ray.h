#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

static inline Ray ray_from(const Vec3 origin, const Vec3 direction) {
    return (Ray){.origin = origin, .direction = direction};
}

Vec3 ray_at(const Ray *ray, double t);

#endif // RAY_H
