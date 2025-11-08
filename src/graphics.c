#include "graphics.h"

#include "render/camera.h"
#include "render/hittables.h"

#include <pthread.h>

#ifdef MAKE_PPM
#include <stdio.h>
#endif // MAKE_PPM

#ifdef MAKE_SDL
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

#include <SDL2/SDL.h>
#include <stdio.h>

static SDL_Window *sdl_window;
static SDL_Surface *sdl_surface;

const int sdl_scale = 2;
#endif // MAKE_SDL

static int image_width;
static int image_height;

bool init_graphics(const int width, const double aspect_ratio) {
    image_width = width;
    image_height = (int)((double)width / aspect_ratio);
#ifdef MAKE_SDL
    SDL_Renderer *sdl_renderer;
    SDL_CreateWindowAndRenderer(image_width * sdl_scale,
                                image_height * sdl_scale, 0, &sdl_window,
                                &sdl_renderer);
    sdl_surface = SDL_GetWindowSurface(sdl_window);
    if (sdl_window == NULL || sdl_surface == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        return false;
    }
#endif // MAKE_SDL
#ifdef MAKE_TICE
    // Make Tice
#endif // MAKE_TICE
#ifdef MAKE_PPM
    printf("P3\n%d %d\n255\n", image_width, image_height);
#endif // MAKE_PPM
    return true;
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
#endif // MAKE_SDL
#ifdef MAKE_TICE
    // Make Tice
#endif // MAKE_TICE
#ifdef MAKE_PPM
    printf("%d %d %d\n", r, g, b);
#endif // MAKE_PPM
}

static inline void set_pixel_row(int y, const Camera *camera,
                                 const Hittables *world) {
    static pthread_mutex_t set_pixel_mutex = PTHREAD_MUTEX_INITIALIZER;
    for (int x = 0; x < image_width; x++) {
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE])
            break;
        Vec3 color = get_pixel(camera, world, (double)x / (double)image_width,
                               (double)y / (double)image_height);
        pthread_mutex_lock(&set_pixel_mutex);
        set_pixel(x, y, color);
        SDL_PumpEvents();
        pthread_mutex_unlock(&set_pixel_mutex);
    }
}

typedef struct {
    const Camera *camera;
    const Hittables *world;
} render_thread_args;

void *renderer_thread(void *arg) {
    static pthread_mutex_t current_row_mutex = PTHREAD_MUTEX_INITIALIZER;
    static int current_row = 0;
    int y;

    const render_thread_args *args = (render_thread_args *)arg;
    for (;;) {
        pthread_mutex_lock(&current_row_mutex);
        y = current_row++;
        pthread_mutex_unlock(&current_row_mutex);

        if (y >= image_height)
            break;
        set_pixel_row(y, args->camera, args->world);
    }

    return NULL;
}

void render(const Camera *camera, const Hittables *world) {
    const int threads_limit = 100;
    pthread_t threads[threads_limit];
    render_thread_args args = {camera, world};
    for (int i = 0; i < threads_limit; i++) {
        pthread_create(&threads[i], NULL, renderer_thread, &args);
    }
    for (int i = 0; i < threads_limit; i++) {
        pthread_join(threads[i], NULL);
    }
}

void stop_graphics(void) {
#ifdef MAKE_SDL
    SDL_UpdateWindowSurface(sdl_window);

    for (; !SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]; SDL_PumpEvents())
        ;

    SDL_FreeSurface(sdl_surface);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
#endif // MAKE_SDL
}
