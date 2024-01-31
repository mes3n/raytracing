#ifndef SPHERE_H
#define SPHERE_H

#include "hit_record.h"
#include "interval.h"
#include "ray.h"
#include "scatter.h"
#include "vec3.h"

#include <stdbool.h>

void set_face_normal(HitRecord *hit_record, const Ray *ray, const Vec3 out_n);

typedef struct {
    Vec3 center;
    double radius;
    Scatterer scatterer;
} Sphere;

bool sphere_hit(const Sphere *sphere, const Ray *ray, const Interval *rayt,
                HitRecord *hit_record, Scatterer *scatterer);

#endif // SPHERE_H
