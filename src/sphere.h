#ifndef SPHERE_H
#define SPHERE_H

#include "ray.h"
#include "vec3.h"

typedef struct {
    Vec3 point;
    Vec3 normal;
    double t;
    int front_face;
} HitRecord;

void set_face_normal(HitRecord *hit_record, const Ray *ray, const Vec3 out_n);

typedef struct {
    Vec3 center;
    double radius;
} Sphere;

int sphere_hit(const Sphere *sphere, const Ray *ray, const double rayt_min,
               const double rayt_max, HitRecord *hit_record);

#endif // SPHERE_H
