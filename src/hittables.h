#ifndef HITTABLES_H
#define HITTABLES_H

#include "hit_record.h"
#include "interval.h"
#include "material.h"
#include "ray.h"

#include <stdbool.h>

typedef bool (*ShapeHitFn)(const void *, const Ray *, const Interval *,
                           HitRecord *, Material **);

typedef struct Hittables {
    const void *shape;
    ShapeHitFn shape_hit;

    struct Hittables *next;
} Hittables;

bool hit_any(const Hittables *hittables, const Ray *ray, const Interval *rayt,
             HitRecord *hit_record, Material **material);

int hittables_add(Hittables **hittables, const void *shape,
                  ShapeHitFn shape_hit);

int hittables_len(Hittables *ht);

void hittables_clear(Hittables *hittables);

#endif // HITTABLES_H
