#ifndef BBOX_H
#define BBOX_H

#include "interval.h"
#include "ray.h"
#include "vec3.h"

#include <stdbool.h>
#include <stddef.h>

/**
 * Struct to represent an axis aligned bounding box.
 *
 * @param x,y,z Intervals of each axis the `Bbox` covers
 */
typedef struct {
    Interval x, y, z;
} Bbox;

#define axis_of(a) offsetof(Bbox, a) / sizeof(Interval)

/**
 * Create a Bbox from two positions.
 * The positions given should be two opposite corners of the Bbox.
 *
 * @param a The first corner
 * @param b The second opposite corner
 *
 * @return A bounding box with corners a and b
 */
static inline Bbox bbox_from_positions(const Vec3 a, const Vec3 b) {
    return (Bbox){
        .x = a.x < b.x ? interval_from(a.x, b.x) : interval_from(b.x, a.x),
        .y = a.y < b.y ? interval_from(a.y, b.y) : interval_from(b.y, a.y),
        .z = a.z < b.z ? interval_from(a.z, b.z) : interval_from(b.z, a.z)};
}

/**
 * Create a `Bbox` by combining two.
 * The created `Bbox` will be the minimal `Bbox` which contains the two others.
 *
 * @param a The first `Bbox`
 * @param b The second `Bbox`
 *
 * @return A `Bbox` minimally containing a and b
 */
static inline Bbox bbox_combine(const Bbox *a, const Bbox *b) {
    return (Bbox){
        .x = interval_combine(a->x, b->x),
        .y = interval_combine(a->y, b->y),
        .z = interval_combine(a->z, b->z),
    };
}

/**
 * Get the longest axis of a `Bbox`.
 * The axis is represent by a number which can be used to calculate
 * which `Interval` of the `Bbox` should be used with pointer arithmetic.
 *
 * @param bbox The `Bbox` to calculate for
 *
 * @return A 0 <= number <= 2 representing the biggest axis
 */
static inline int bbox_max_axis(const Bbox *bbox) {
    const double len_x = bbox->x.max - bbox->x.min;
    const double len_y = bbox->y.max - bbox->y.min;
    const double len_z = bbox->z.max - bbox->z.min;

    return len_x > len_y ? (len_x > len_z ? axis_of(x) : axis_of(z))
                         : (len_y > len_z ? axis_of(y) : axis_of(z));
}

/**
 * Calculate if a ray hits a `Bbox` within a specific interval. If so, return
 * true.
 *
 * @param bbox A pointer the `Bbox` to check
 * @param ray A pointer to the `Ray` which should be checked
 * @param rayt An interval for ray
 *
 * @return True if hit
 */
bool bbox_hit(const Bbox *bbox, const Ray *ray, Interval rayt);

#endif // BBOX_H
