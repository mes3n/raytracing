#ifndef HITTABLE_H
#define HITTABLE_H

#include "../hit_record.h"
#include "../material.h"

#include "../math/bbox.h"
#include "../math/interval.h"
#include "../math/ray.h"

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

#endif // HITTABLE_H
