#ifndef TEXTURE_H
#define TEXTURE_H

#include "math/vec3.h"

typedef Vec3 (*TextureColorFn)(const void *texture, const double u,
                               const double v, const Vec3 p);

#define DERIVE_TEXTURE() TextureColorFn color;

typedef struct {
    DERIVE_TEXTURE()
} Texture;

typedef struct {
    DERIVE_TEXTURE()
    Vec3 albedo;
} SolidTexture;

SolidTexture *new_solid_texture(const Vec3 albedo);

void free_solid_texture(SolidTexture *solid_texture);

Vec3 solid_texture_color(SolidTexture *solid_texture, const double u,
                         const double v, const Vec3 p);

typedef struct {
    DERIVE_TEXTURE()
    double inv_scale;
    const Texture *even;
    const Texture *odd;
} CheckerTexture;

CheckerTexture *new_checker_texture(const Texture *even, const Texture *odd);

void free_checker_texture(CheckerTexture *checker_texture);

Vec3 checker_texture_color(CheckerTexture *checker_texture, const double u,
                           const double v, const Vec3 p);

#endif // TEXTURE_H
