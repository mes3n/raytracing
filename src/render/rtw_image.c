#include "rtw_image.h"

#include "math/vec3.h"

#include <assert.h>

#ifndef NO_STB_IMAGE
#include <stdlib.h>

#include "stb_image.h"
#else
#include <stdio.h>
#endif // NO_STB_IMAGE

#define bytes_per_pixel 3

static inline int clamp_int(const int min, const int max, const int x) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

RwtImage *new_rwt_image(const char *filename
#ifdef NO_STB_IMAGE
                        __attribute__((unused))
#endif // NO_STB_IMAGE
) {
#ifdef NO_STB_IMAGE
    fprintf(stderr, "[ WARN ] Using stb_image on NO_STB_IMAGE build.\n");
    return NULL;
#else
    RwtImage *rwt_image = (RwtImage *)malloc(sizeof(RwtImage));
    *rwt_image = (RwtImage){0};

    int n = 0;
    rwt_image->data = stbi_load(filename, &rwt_image->width, &rwt_image->height,
                                &n, bytes_per_pixel);

    assert(rwt_image->data != NULL);
    assert(n == bytes_per_pixel);

    return rwt_image;
#endif // NO_STB_IMAGE
}

Vec3 rtw_image_pixel(const RwtImage *image, int x, int y) {
#ifndef NO_STB_IMAGE
    if (image->data == NULL)
#endif                                   // NO_STB_IMAGE
        return vec3_from(1.0, 0.0, 1.0); // Debugging color magenta

    x = clamp_int(0, image->width, x);
    y = clamp_int(0, image->height, y);

    unsigned char *pixel =
        image->data + (y * image->width + x) * bytes_per_pixel;
    return vec3_from((double)pixel[0] / 255.0, (double)pixel[1] / 255.0,
                     (double)pixel[2] / 255.0);
}
