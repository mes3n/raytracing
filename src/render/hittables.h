#ifndef HITTABLES_H
#define HITTABLES_H

#include "hit_record.h"
#include "interval.h"
#include "material.h"
#include "ray.h"

#include <stdbool.h>

/// Function pointer type to describe a function which must be implemented by
/// a shape in order for its ray intersections to be calculated.
typedef bool (*ShapeHitFn)(const void *shape, const Ray *ray,
                           const Interval *rayt, HitRecord *hr);

/**
 * All shape structs must implement these fields first and in correct order.
 *
 * @param material Pointer to material shape is made of
 * @param hit Function pointer to calculate if shape has been hit
 */
typedef struct {
    Material *material;
    ShapeHitFn hit;
} Hittable;

/**
 * Linked list implementation of hittable objects.
 *
 * @param shape A hittable object implemented according to `Hittable`
 * @param next Next object in linked list
 */
typedef struct Hittables {
    const Hittable *shape;
    struct Hittables *next;
} Hittables;

/**
 * Calculate if any object in the hittables list has been hit. If so, return
 * true and set the `hit_record` and `material` pointers as per the shape that
 * was hit. Uses the closest hit if there are multiple. Runs in linear time.
 *
 * @param hittables Linked list of shapes to search for hits.
 * @param ray Ray to check for collisions
 * @param rayt The minimum and maximum lengths of ray
 * @param hit_record Pointer to object where hit_record will be stored
 * @param material Pointer to object where material pointer will be stored
 *
 * @return True if hit
 */
bool hit_any(const Hittables *hittables, const Ray *ray, const Interval *rayt,
             HitRecord *hit_record, Material **material);

/**
 * Add a hittable object to the hittables LL.
 * `shape` MUST be implemented as per `Hittable`.
 *
 * @return False if failed
 */
bool hittables_add(Hittables **hittables, const void *shape)
    __attribute__((nonnull));

int hittables_len(Hittables *ht);

/**
 * Remove and free all elements from LL.
 *
 * Note that the shape object itself will not be freed.
 */
void hittables_clear(Hittables *hittables);

#endif // HITTABLES_H
