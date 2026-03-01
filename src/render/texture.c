#include "texture.h"

#include "math/vec3.h"

#include <math.h>
#include <stdlib.h>

SolidTexture *new_solid_texture(const Vec3 albedo) {
    SolidTexture *solid_texture = (SolidTexture *)malloc(sizeof(SolidTexture));
    *solid_texture = (SolidTexture){
        .color = (TextureColorFn)solid_texture_color, .albedo = albedo};
    return solid_texture;
}

Vec3 solid_texture_color(SolidTexture *solid_texture,
                         __attribute__((unused)) const double u,
                         __attribute__((unused)) const double v,
                         __attribute__((unused)) const Vec3 p) {
    return solid_texture->albedo;
}

CheckerTexture *new_checker_texture(const Texture *even, const Texture *odd) {
    CheckerTexture *checker_texture =
        (CheckerTexture *)malloc(sizeof(CheckerTexture));
    *checker_texture =
        (CheckerTexture){.color = (TextureColorFn)checker_texture_color,
                         .inv_scale = 1.0 / 0.5,
                         .even = even,
                         .odd = odd};
    return checker_texture;
}

Vec3 checker_texture_color(CheckerTexture *checker_texture, const double u,
                           const double v, const Vec3 p) {
    const int x = (int)floor(p.x * checker_texture->inv_scale);
    const int y = (int)floor(p.y * checker_texture->inv_scale);
    const int z = (int)floor(p.z * checker_texture->inv_scale);

    return (x + y + z) % 2
               ? checker_texture->even->color(checker_texture->even, u, v, p)
               : checker_texture->odd->color(checker_texture->odd, u, v, p);
}
