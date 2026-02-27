#ifndef BVH_H
#define BVH_H

#include "bbox.h"
#include "hit_record.h"
#include "hittables.h"
#include "interval.h"
#include "material.h"
#include "ray.h"

#include <stdbool.h>

/**
 * Struct representing a bounding volume hierarchy node.
 *
 * @param bbox The `Bvh` node's `Bbox`
 * @param left Its left child, NULL if leaf
 * @param right Its right child, NULL if leaf
 * @param shape A pointer to its shape, NULL unless leaf
 */
typedef struct Bvh {
    Bbox bbox;
    struct Bvh *left;
    struct Bvh *right;
    const Hittable *shape;
} Bvh;

/**
 * Create a dynamically allocated `Bvh` from a `Hittables` linked list.
 * The `Hittables` list will be consumed and freed.
 *
 * @param hittables A `Hittables` implemented LL to convert
 *
 * @return A dynamically allocated `Bvh` tree root node
 */
Bvh *bvh_from_hittables(Hittables *hittables);

/**
 * Count the leaves in a `Bvh`.
 *
 * @param bvh Root node of the `Bvh`.
 *
 * @return The count of leaves
 */
int bvh_count_leaves(const Bvh *bvh);

/**
 * Free all nodes of a `Bvh`.
 * Note that this does not free the node's contents.
 *
 * @param bvh Root node of `Bvh`
 */
void bvh_free(Bvh *bvh);


/**
 * Calculate if a ray hits a `Bvh` within a specific interval. If so, return
 * true and store the `HitRecord` and `Material` of the hit.
 *
 * @param bvh Root node of the `Bvh`
 * @param ray A pointer to the ray which should be checked
 * @param rayt An interval for ray
 * @param hit_record A pointer for where the store the `HitRecord`
 * @param material A pointer the where to store the pointer of the `Material`
 *
 * @return True if hit
 */
bool bvh_hit(const Bvh *bvh, const Ray *ray, const Interval *rayt,
             HitRecord *hit_record, Material **material);

#endif // BVH_H
