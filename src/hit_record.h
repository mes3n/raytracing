#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "vec3.h"


typedef struct {
    Vec3 point;
    Vec3 normal;
    double t;
    int front_face;
} HitRecord;

#endif // HIT_RECORD_H
