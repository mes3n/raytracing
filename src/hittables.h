#ifndef HITTABLES_H
#define HITTABLES_H

#include "ray.h"
#include "sphere.h"

#define NULL (void *)0

typedef int (*ShapeHitFn)(const void *, const Ray *, const double, const double,
                          HitRecord *);

typedef struct Hittables {
    const void *shape;
    ShapeHitFn shape_hit;

    struct Hittables *next;
} Hittables;

int hit_any(const Hittables *hittables, const Ray *ray, const double ray_tmin,
            const double ray_tmax, HitRecord *hit_record);

int hittables_add(Hittables **hittables, const void *shape,
                         ShapeHitFn shape_hit);

int hittables_len(Hittables *ht);

void hittables_clear(Hittables *hittables);

#endif // HITTABLES_H
