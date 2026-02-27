#include "bbox.h"
#include "interval.h"

bool bbox_hit(const Bbox *bbox, const Ray *ray, Interval rayt) {
    double *ray_dir = (double *)&ray->direction;
    double *ray_orig = (double *)&ray->origin;

    for (int axis = 0; axis < 3; axis++) {
        const Interval *ax = (Interval *)bbox + axis;
        const double adinv = 1.0 / ray_dir[axis];

        double t0 = (ax->min - ray_orig[axis]) * adinv;
        double t1 = (ax->max - ray_orig[axis]) * adinv;

        if (t0 < t1) {
            if (t0 > rayt.min)
                rayt.min = t0;
            if (t1 < rayt.max)
                rayt.max = t1;
        } else {
            if (t1 > rayt.min)
                rayt.min = t1;
            if (t0 < rayt.max)
                rayt.max = t0;
        }

        if (rayt.max <= rayt.min)
            return false;
    }
    return true;
}
