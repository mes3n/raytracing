#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "render/camera.h"
#include "render/hittables.h"

#include <stdbool.h>

bool init_graphics(const int image_width, const int image_height);
void render(const Camera *camera, const Hittables *world);
void stop_graphics(void);

#endif // GRAPHICS_H
