#ifndef SCATTER_H
#define SCATTER_H

#include "hit_record.h"
#include "ray.h"

#include <stdbool.h>

typedef bool (*ScatterFunc)(const Ray *ray, const HitRecord *hr,
                            Ray *scattered);

typedef struct {
    ScatterFunc scatter;
    Vec3 attenuation;
} Scatterer;

bool lambertian_scatter(const Ray *ray, const HitRecord *hr, Ray *scattered);

bool metal_scatter(const Ray *ray, const HitRecord *hr, Ray *scattered);

#endif // SCATTER_H
