#ifndef SPHERE_H
#define SPHERE_H

#include "hit_record.h"
#include "interval.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

#include <stdbool.h>

void set_face_normal(HitRecord *hit_record, const Ray *ray, const Vec3 out_n);

typedef struct {
    Vec3 center;
    double radius;
    Material *material;
} Sphere;

bool sphere_hit(const Sphere *sphere, const Ray *ray, const Interval *rayt,
                HitRecord *hit_record, Material **material);

#endif // SPHERE_H
