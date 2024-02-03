#ifndef CAMERA_H
#define CAMERA_H

#include "hittables.h"
#include "vec3.h"

typedef struct {
    Vec3 dx, dy;
    Vec3 pos_at00;
} Viewport;

typedef struct {
    int image_width, image_height;
    int samples_per_pixel;
    int max_depth;

    Vec3 origin;

    double defocus_angle;
    Vec3 defocus_disc_u;
    Vec3 defocus_disc_v;

    Viewport viewport;
} Camera;

int init_camera(Camera *camera);
void render(const Camera *camera, const Hittables *world);

#endif // CAMERA_H
