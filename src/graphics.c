#include "graphics.h"

#include <stdio.h>

void init_graphics(const int image_width, const int image_height) {
#ifdef MAKE_SDL
    // Init SDL
#endif
#ifdef MAKE_TICE
#endif
#ifdef MAKE_PPM
    printf("P3\n%d %d\n255\n", image_width, image_height);
#endif
}

#ifndef MAKE_PPM
void set_pixel(int x, int y, Vec3 rgb) {
#ifdef MAKE_SDL
    // SDL Stuff
    printf("Hello SDL\n");
#endif
#ifdef MAKE_TICE
    // TICE Stuff
    printf("Hello TICE\n");
#endif
}
#else
void set_pixel(Vec3 rgb) {
    int r = (int)(255.999 * rgb.x);
    int g = (int)(255.999 * rgb.y);
    int b = (int)(255.999 * rgb.z);
    printf("%d %d %d\n", r, g, b);
}
#endif
