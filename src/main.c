#include "graphics.h"

#include <stdio.h>

const int image_width = 256u;
const int image_height = 256u;

int main(int argc, char **argv) {

    init_graphics();

    for (int y = 0; y < image_height; y++) {
        fprintf(stderr, "\rScanlines remaining: %d ", image_height - y);
        fflush(stderr);
        for (int x = 0; x < image_width; x++) {
            vec3 rgb;

            rgb.x = (double)x / (double)image_width;
            rgb.y = (double)y / (double)image_height;
            rgb.z = 0.0;

#ifndef MAKE_PPM
            set_pixel(x, y, rgb);
#else
            set_pixel(rgb);
#endif
        }
    }
    fprintf(stderr, "\rDone.                 \n");

    return 0;
}
