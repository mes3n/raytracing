#ifndef MATERIAL_H
#define MATERIAL_H

#include "hit_record.h"
#include "texture.h"

#include "math/ray.h"

#include <stdbool.h>

/// Function pointer type to describe a function which must be implemented by
/// a material to describe its effect on the incoming ray
typedef bool (*ScatterFn)(const void *material, const Ray *ray,
                          const HitRecord *hr, Ray *scattered,
                          Vec3 *attenuation);

#define DERIVE_MATERIAL() ScatterFn scatter;

/**
 * All material structs must implement this field first.
 *
 * @param scatter Function pointer to calculate how ray is affected by material
 */
typedef struct {
    DERIVE_MATERIAL()
} Material;

/// Matt material that evenly distributes incoming rays
typedef struct {
    DERIVE_MATERIAL()
    Texture *texture;
} Lambertian;

Lambertian *new_lambertian(Texture *texture);

Lambertian *new_lambertian_solid(const Vec3 albedo);

bool lambertian_scatter(const Lambertian *lambertian, const Ray *ray,
                        const HitRecord *hr, Ray *scattered, Vec3 *attenuation);

/// Reflective material where ray bounce mirrored to the normal
/// Fuzz describes the randomness of the scattered rays direction
typedef struct {
    DERIVE_MATERIAL()
    Vec3 albedo;
    double fuzz;
} Metal;

Metal *new_metal(const Vec3 albedo, const double fuzz);

bool metal_scatter(const Metal *metal, const Ray *ray, const HitRecord *hr,
                   Ray *scattered, Vec3 *attenuation);

/// Glass like transparent material
typedef struct {
    DERIVE_MATERIAL()
    double refraction;
} Dielectric;

Dielectric *new_dielectric(const double refraction);

bool dielectric_scatter(const Dielectric *dielectric, const Ray *ray,
                        const HitRecord *hr, Ray *scattered, Vec3 *attenuation);

#endif // MATERIAL_H
