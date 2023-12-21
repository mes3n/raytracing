#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"

typedef struct {
    double width, height;
    Vec3 x, y, dx, dy;
    Vec3 top_left;
    Vec3 pos_at00;
} Viewport;

typedef struct {
    double focal_length;
    Viewport viewport;
    Vec3 origin;
} Camera;

void init_camera(Camera *camera, const int image_width, const int image_height);

#endif // CAMERA_H
