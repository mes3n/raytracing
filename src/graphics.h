#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "render/camera.h"
#include "render/hittables.h"

#include <stdbool.h>

/**
 * Initialize the graphics backend
 *
 * @param width Sets the width of the generated image
 * @param aspect_ratio Sets the aspect ratio of the generated image
 *
 * @return Boolen true for success
 */
bool init_graphics(const int width, const double aspect_ratio);

/**
 * Render the image seen by camera in world
 *
 * @param camera The camera viewing the world
 * @param world The world to be rendered
 */
void render(const Camera *camera, const Hittables *world);

/**
 * Finish graphic routines
 */
void stop_graphics(void);

#endif // GRAPHICS_H
