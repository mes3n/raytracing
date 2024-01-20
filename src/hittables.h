#ifndef HITTABLES_H
#define HITTABLES_H

#include "hit_record.h"
#include "interval.h"
#include "ray.h"

#define NULL (void *)0

typedef int (*ShapeHitFn)(const void *, const Ray *, const Interval *,
                          HitRecord *);

typedef struct Hittables {
    const void *shape;
    ShapeHitFn shape_hit;

    struct Hittables *next;
} Hittables;

int hit_any(const Hittables *hittables, const Ray *ray, const Interval *rayt,
            HitRecord *hit_record);

int hittables_add(Hittables **hittables, const void *shape,
                  ShapeHitFn shape_hit);

int hittables_len(Hittables *ht);

void hittables_clear(Hittables *hittables);

#endif // HITTABLES_H
