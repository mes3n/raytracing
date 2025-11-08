#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/**
 * Struct to represent a ray in space using its origin and direction.
 */
typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

static inline Ray ray_from(const Vec3 origin, const Vec3 direction) {
    return (Ray){.origin = origin, .direction = direction};
}

/**
 * Calculate the position of ray after traveling `t` lengths.
 */
Vec3 ray_at(const Ray *ray, double t);

#endif // RAY_H
