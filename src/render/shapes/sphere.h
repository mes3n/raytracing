#ifndef SPHERE_H
#define SPHERE_H

#include "../bbox.h"
#include "../hittables.h"

#include <stdbool.h>

typedef struct {
    DERIVE_HITTABLE()

    Vec3 center;
    Vec3 dcenter;
    double radius;
} Sphere;

Sphere sphere_from(const Material *material, const Vec3 center,
                   const Vec3 dcenter, const double radius);

bool sphere_hit(const Sphere *sphere, const Ray *ray, const Interval *rayt,
                HitRecord *hit_record);

Bbox sphere_bbox(const Sphere *sphere);

#endif // SPHERE_H
