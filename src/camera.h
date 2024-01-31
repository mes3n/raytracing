#ifndef CAMERA_H
#define CAMERA_H

#include "hittables.h"
#include "vec3.h"

typedef struct {
    double width, height;
    Vec3 x, y, dx, dy;
    Vec3 top_left;
    Vec3 pos_at00;
} Viewport;

typedef struct {
    int image_width, image_height;
    int samples_per_pixel;
    int max_depth;
    double focal_length;
    Viewport viewport;
    Vec3 origin;
} Camera;

int init_camera(Camera *camera);
void render(const Camera *camera, const Hittables *world);

#endif // CAMERA_H
