#ifndef QUAD_H
#define QUAD_H

#include "hittables.h"
#include "material.h"

typedef struct {
    Material *material;
    ShapeHitFn hit;

    Vec3 origin;
    Vec3 u, v;

    Vec3 normal;
    Vec3 w;
    double d;
} Quad;

Quad quad_from(Material *material, const Vec3 origin, const Vec3 u,
               const Vec3 v);

bool quad_hit(const Quad *quad, const Ray *ray, const Interval *rayt,
              HitRecord *hit_record);

#endif // QUAD_H
