#include "rtw_image.h"

#include "math/vec3.h"

#include <assert.h>
#include <stdlib.h>

#define STBI_NO_FAILURE_STRINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define bytes_per_pixel 3

static inline int clamp_int(const int min, const int max, const int x) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

RwtImage *new_rwt_image(const char *filename) {
    RwtImage *rwt_image = (RwtImage *)malloc(sizeof(RwtImage));
    *rwt_image = (RwtImage){0};

    int n = 0;
    rwt_image->data = stbi_load(filename, &rwt_image->width, &rwt_image->height,
                                &n, bytes_per_pixel);

    assert(rwt_image->data != NULL);
    assert(n == bytes_per_pixel);

    return rwt_image;
}

Vec3 rtw_image_pixel(const RwtImage *image, int x, int y) {
    if (image->data == NULL)
        return vec3_from(1.0, 0.0, 1.0);  // Debugging color magenta

    x = clamp_int(0, image->width, x);
    y = clamp_int(0, image->height, y);

    unsigned char *pixel =
        image->data + (y * image->width + x) * bytes_per_pixel;
    return vec3_from((double)pixel[0] / 255.0, (double)pixel[1] / 255.0,
                     (double)pixel[2] / 255.0);
}
