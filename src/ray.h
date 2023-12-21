#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

Vec3 ray_at(const Ray *ray, double t);

#endif // RAY_H
