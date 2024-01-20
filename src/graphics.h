#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vec3.h"

int init_graphics(const int image_width, const int image_height);
void set_pixel(int x, int y, Vec3 rgb);
void stop_graphics(void);

#endif // GRAPHICS_H
