#ifndef SPHERE_H
#define SPHERE_H

#include "hittables.h"

#include <stdbool.h>

typedef struct {
    Material *material;
    ShapeHitFn hit;

    Vec3 center;
    Vec3 dcenter;
    double radius;
} Sphere;

bool sphere_hit(const Sphere *sphere, const Ray *ray, const Interval *rayt,
                HitRecord *hit_record);

#endif // SPHERE_H
