#ifndef HITTABLES_H
#define HITTABLES_H

#include "hit_record.h"
#include "material.h"

#include "math/bbox.h"
#include "math/interval.h"
#include "math/ray.h"

#include <stdbool.h>

/// Function pointer types to describe a functions which must be
/// implemented by a shape

/// Check if the shape was hit
typedef bool (*ShapeHitFn)(const void *shape, const Ray *ray,
                           const Interval *rayt, HitRecord *hr);

/**
 * The base requirements for a `Hittable`.
 * Placing this macro first inside the struct which should be a `Hittable`
 * implements the proper fields.
 */
#define DERIVE_HITTABLE()                                                      \
    const Material *material;                                                  \
    ShapeHitFn hit;                                                            \
    Bbox bbox;

/**
 * All shape structs must implement these fields first and in correct order.
 * This can be done with the `DERIVE_HITTABLE()` macro.
 *
 * @param material Pointer to material shape is made of
 * @param hit Function pointer to calculate if shape has been hit
 * @pararm bbox The bounding box of the `Hittable`
 */
typedef struct {
    DERIVE_HITTABLE()
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
 * @param hittables Linked list of shapes to search for hits
 * @param ray Ray to check for collisions
 * @param rayt The minimum and maximum lengths of ray
 * @param hit_record Pointer to object where hit_record will be stored
 * @param material Pointer to object where material pointer will be stored
 *
 * @return True if hit
 */
[[deprecated]] bool hit_any(const Hittables *hittables, const Ray *ray,
                            const Interval *rayt, HitRecord *hit_record,
                            Material **material);

/**
 * Add a hittable object to the hittables LL.
 * `shape` MUST be implemented as per `Hittable`.
 *
 * @param hittables A pointer to the root node
 * @param shape A pointer to the shape which will be added
 *
 * @return False if failed
 */
bool hittables_add(Hittables **hittables, const void *shape)
    __attribute__((nonnull));

Bbox bbox_from_hittables(const Hittables *hittables) __attribute__((nonnull));

Hittables *hittables_split(Hittables *hittables);

Hittables *hittables_sort(Hittables *hittables, const int axis);

/**
 * Get the length of the hittables LL.
 *
 * @param ht The root node
 *
 * @returns The count of elements in LL
 */
int hittables_len(const Hittables *hittables);

/**
 * Remove and free all elements from LL.
 *
 * Note that the shape object itself will not be freed.
 *
 * @param hittables The root node
 */
void hittables_clear(Hittables *hittables);

#endif // HITTABLES_H
