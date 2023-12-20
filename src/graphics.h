#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vec3.h"

#ifndef NO_MAKE_SDL
#define MAKE_SDL
#endif

extern const int image_width;
extern const int image_height;

void init_graphics(void);

#ifndef MAKE_PPM
void set_pixel(int x, int y, vec3 rgb);
#else
void set_pixel(vec3 rgb);
#endif

#endif // GRAPHICS_H
