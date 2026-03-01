#ifndef RTW_IMAGE_H
#define RTW_IMAGE_H

#include "math/vec3.h"

typedef struct {
    unsigned char *data;
    int width;
    int height;
} RwtImage;

RwtImage *new_rwt_image(const char *filename);

Vec3 rtw_image_pixel(const RwtImage *image, const int x, const int y);

#endif // RTW_IMAGE_H
