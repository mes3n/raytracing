#include "graphics.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

#ifdef MAKE_PPM
#include <stdio.h>
#endif

#ifdef MAKE_SDL
#include <SDL2/SDL.h>
#include <stdio.h>

SDL_Window *sdl_window;
SDL_Surface *sdl_surface;

const int sdl_scale = 2;
#endif

int init_graphics(const int image_width, const int image_height) {
#ifdef MAKE_SDL
    SDL_Renderer *sdl_renderer;
    SDL_CreateWindowAndRenderer(image_width * sdl_scale,
                                image_height * sdl_scale, 0, &sdl_window,
                                &sdl_renderer);
    sdl_surface = SDL_GetWindowSurface(sdl_window);
    if (sdl_window == NULL || sdl_surface == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        return -1;
    }
#endif
#ifdef MAKE_TICE
    // Make Tice
#endif
#ifdef MAKE_PPM
    printf("P3\n%d %d\n255\n", image_width, image_height);
#endif
    return 0;
}

static inline Vec3 linear_to_gamma(Vec3 v) {
    return vec3_from(sqrt(v.x), sqrt(v.y), sqrt(v.z));
}

void set_pixel(int x, int y, Vec3 rgb) {
    rgb = linear_to_gamma(rgb);
    int r = (int)(255.999 * rgb.x);
    int g = (int)(255.999 * rgb.y);
    int b = (int)(255.999 * rgb.z);
#ifdef MAKE_SDL
    SDL_Rect rect = (SDL_Rect){
        .x = x * sdl_scale,
        .y = y * sdl_scale,
        .w = sdl_scale,
        .h = sdl_scale,
    };

    SDL_FillRect(sdl_surface, &rect, SDL_MapRGB(sdl_surface->format, r, g, b));
    SDL_UpdateWindowSurface(sdl_window);
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
    SDL_UpdateWindowSurface(sdl_window);

    for (; !SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]; SDL_PumpEvents())
        ;

    SDL_FreeSurface(sdl_surface);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
#endif
}
