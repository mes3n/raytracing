#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "ray.h"
#include "vec3.h"

#include <stdbool.h>

/**
 * Public struct to represent the values generated when a ray hits an object
 * in the world.
 *
 * @param point A point in space where the ray intersected an object
 * @param normal Normal vector of the object where it was hit by the ray
 * @param t Distance traveled by the ray
 * @param front_face Was the hit on the back or front of object
 */
typedef struct {
    Vec3 point;
    Vec3 normal;
    double t;
    bool front_face;
} HitRecord;

static inline void set_face_normal(HitRecord *hit_record, const Ray *ray,
                                   const Vec3 out_n) {
    hit_record->front_face = vec3_dot(ray->direction, out_n) < 0;
    hit_record->normal =
        hit_record->front_face ? out_n : vec3_scale(out_n, -1.0);
}

#endif // HIT_RECORD_H
