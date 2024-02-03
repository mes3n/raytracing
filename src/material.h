#ifndef MATERIAL_H
#define MATERIAL_H

#include "hit_record.h"
#include "ray.h"

#include <stdbool.h>

typedef bool (*ScatterFn)(const void *, const Ray *, const HitRecord *, Ray *,
                          Vec3 *);

typedef struct {
    ScatterFn scatter;
} Material;

typedef struct {
    ScatterFn scatter;
    Vec3 albedo;
} Lambertian;

bool lambertian_scatter(const Lambertian *lambertian, const Ray *ray,
                        const HitRecord *hr, Ray *scattered, Vec3 *attenuation);

typedef struct {
    ScatterFn scatter;
    Vec3 albedo;
    double fuzz;
} Metal;

bool metal_scatter(const Metal *metal, const Ray *ray, const HitRecord *hr,
                   Ray *scattered, Vec3 *attenuation);

typedef struct {
    ScatterFn scatter;
    double refraction;
} Dielectric;

bool dielectric_scatter(const Dielectric *dielectric, const Ray *ray,
                        const HitRecord *hr, Ray *scattered, Vec3 *attenuation);

#endif // MATERIAL_H
