#ifndef CAMERA_H
#define CAMERA_H

#include "hittables.h"
#include "vec3.h"

#include <stdbool.h>

/**
 * Internal struct to represent to camera's viewport
 */
typedef struct {
    Vec3 u, v;
    Vec3 pos_at00;
} Viewport;

/**
 * Struct to represent a `Camera` object
 *
 * The struct must be initialized using the `init_camera` function
 * before it can be used. All fields will be set automatically and should
 * not be changed manually.
 */
typedef struct {
    int samples_per_pixel;
    int max_depth;

    Vec3 origin;

    double defocus_angle;
    Vec3 defocus_disc_u;
    Vec3 defocus_disc_v;

    Viewport viewport;
} Camera;

/**
 * Function to initialize to `Camera` object.
 *
 * @param camera Pointer to `Camera` object
 * @param aspect_ratio Aspect ratio of viewport
 * @param sampels Sample rays created per pixel
 *
 * @return Boolean true for success else false
 */
bool init_camera(Camera *camera, const double aspect_ratio,
                 const int sampels);

/**
 * Function to get the color of a specific pixel from where the camera sees the
 * world. The pixel color is calculated by raytracing the world through the
 * configured camera's view.
 *
 * @param camera Pointer to `Camera` object
 * @param world Pointer to `Hittables` world object
 * @param x position of the pixel within the camera's viewport 0.0 < x < 1.0
 * @param y position of the pixel within the camera's viewport 0.0 < y < 1.0
 *
 * @return A floating color value vector where the elements are between 0 and 1
 */
Vec3 get_pixel(const Camera *camera, const Hittables *world, const double x,
               const double y);

#endif // CAMERA_H
