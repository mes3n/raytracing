#include "graphics.h"

#include <SDL2/SDL.h>
#include <stdio.h>

SDL_Window *window;
SDL_Renderer *renderer;

int init_graphics(const int image_width, const int image_height) {
#ifdef MAKE_SDL
    SDL_CreateWindowAndRenderer(image_width, image_height, 0, &window,
                                &renderer);
    if (window == NULL || renderer == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
#endif
#ifdef MAKE_TICE
    // Make Tice
#endif
#ifdef MAKE_PPM
    printf("P3\n%d %d\n255\n", image_width, image_height);
#endif
    return 0;
}

void set_pixel(int x, int y, Vec3 rgb) {
    int r = (int)(255.999 * rgb.x);
    int g = (int)(255.999 * rgb.y);
    int b = (int)(255.999 * rgb.z);
#ifdef MAKE_SDL
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
#endif
#ifdef MAKE_TICE
    // Make Tice
#endif
#ifdef MAKE_PPM
    printf("%d %d %d\n", r, g, b);
#endif
}

void stop_graphics(void) {
#ifdef MAKE_SDL
    SDL_RenderPresent(renderer);

    do {
        SDL_PumpEvents();
    } while (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE] == 0);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif
}
