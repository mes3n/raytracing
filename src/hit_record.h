#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "vec3.h"

#include <stdbool.h>

typedef struct {
    Vec3 point;
    Vec3 normal;
    double t;
    bool front_face;
} HitRecord;

#endif // HIT_RECORD_H
