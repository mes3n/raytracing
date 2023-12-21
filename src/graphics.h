#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vec3.h"

#ifndef NO_MAKE_SDL
#define MAKE_SDL
#endif

void init_graphics(const int image_width, const int image_height);

#ifndef MAKE_PPM
void set_pixel(int x, int y, Vec3 rgb);
#else
void set_pixel(Vec3 rgb);
#endif

#endif // GRAPHICS_H
