#include "texture.h"

#include "math/interval.h"
#include "math/vec3.h"
#include "rtw_image.h"

#include <math.h>
#include <stdlib.h>

#ifdef NO_STB_IMAGE
#include <stdio.h>
#endif // NO_STB_IMAGE

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

ImageTexture *new_image_texture(const char *filename) {
#ifdef NO_STB_IMAGE
#include <stdio.h>
    fprintf(stderr, "[ WARN ] Using image textures on NO_STB_IMAGE build.\n");
    return (ImageTexture *)new_checker_texture(
        (Texture *)new_solid_texture(vec3_from(1.0, 0.0, 1.0)),
        (Texture *)new_solid_texture(vec3_zero()));
#endif // NO_STB_IMAGE
    ImageTexture *image_texture = (ImageTexture *)malloc(sizeof(ImageTexture));
    *image_texture =
        (ImageTexture){.color = (TextureColorFn)image_texture_color,
                       .image = new_rwt_image(filename)};
    return image_texture;
}

Vec3 image_texture_color(ImageTexture *image_texture, double u, double v,
                         __attribute__((unused)) const Vec3 p) {
    if (image_texture->image == NULL || image_texture->image->data == NULL)
        return vec3_from(0.0, 1.0, 1.0); // Cyan if not loaded

    const Interval zero_one = interval_from(0.0, 1.0);
    u = clamp(&zero_one, u);
    v = 1.0 - clamp(&zero_one, v);

    const int x = (int)floor(u * (double)image_texture->image->width);
    const int y = (int)floor(v * (double)image_texture->image->height);
    return rtw_image_pixel(image_texture->image, x, y);
}
