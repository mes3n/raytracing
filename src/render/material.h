#ifndef MATERIAL_H
#define MATERIAL_H

#include "hit_record.h"
#include "ray.h"

#include <stdbool.h>

/// Function pointer type to describe a function which must be implemented by
/// a material to describe its effect on the incoming ray
typedef bool (*ScatterFn)(const void *, const Ray *ray, const HitRecord *hr,
                          Ray *scattered, Vec3 *attenuation);

/**
 * All material structs must implement this field first.
 *
 * @param scatter Function pointer to calculate how ray is affected by material
 */
typedef struct {
    ScatterFn scatter;
} Material;

/// Matt material that evenly distributes incoming rays
typedef struct {
    ScatterFn scatter;
    Vec3 albedo;
} Lambertian;

bool lambertian_scatter(const Lambertian *lambertian, const Ray *ray,
                        const HitRecord *hr, Ray *scattered, Vec3 *attenuation);

/// Reflective material where ray bounce mirrored to the normal
/// Fuzz describes the randomness of the scattered rays direction
typedef struct {
    ScatterFn scatter;
    Vec3 albedo;
    double fuzz;
} Metal;

bool metal_scatter(const Metal *metal, const Ray *ray, const HitRecord *hr,
                   Ray *scattered, Vec3 *attenuation);


/// Glass like transparent material
typedef struct {
    ScatterFn scatter;
    double refraction;
} Dielectric;

bool dielectric_scatter(const Dielectric *dielectric, const Ray *ray,
                        const HitRecord *hr, Ray *scattered, Vec3 *attenuation);

#endif // MATERIAL_H
