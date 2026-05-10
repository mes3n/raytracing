#include "render/bvh.h"
#include "render/camera.h"
#include "render/hittables.h"
#include "render/opencl/loader.h"

#include "graphics.h"
#include "scenes.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static const char help_message[] =
    "Usage: %s\n"
    "Raytracing renderer to create a static image.\n"
    "  -w SCENE    The scene to be rendered as int (default: 0)\n"
    "  -d DIMS     Dimensions of rendered image as INTxINT\n"
    "  -s SAMPLES  The amount of sample rays per pixel (default: 10)\n"
    "  -t THREADS  The amount of threads to render with (default: processing "
    "units)\n"
    "  -z SCALE    The scaleup for the renderd image (default: 1)\n";

bool get_dims(char *str, int *width, int *height) {
    static const int max_string_size = 9;
    char *x = memchr(str, 'x', max_string_size);
    return x != NULL && sscanf(str, "%4d", width) == 1 &&
           sscanf(x + 1, "%4d", height) == 1;
}

long long time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

int sdl_scale = 1;
int main(int argc, char **argv) {
    int scene = 0;
    int width = 350;
    int height = 200;
    int samples = 10;
    int nthreads = sysconf(_SC_NPROCESSORS_ONLN);

    char opt;
    while ((opt = getopt(argc, argv, "hw:d:s:t:z:")) != (char)0xff) {
        switch (opt) {
        case 'w':
            if (sscanf(optarg, "%d", &scene) != 1) {
                fprintf(stderr, "Invalid scene format: %s\n", optarg);
                return 1;
            }
            break;
        case 'd':
            if (!get_dims(optarg, &width, &height)) {
                fprintf(stderr, "Invalid dimension format: %s\n", optarg);
                return 1;
            }
            break;
        case 's':
            if (sscanf(optarg, "%d", &samples) != 1) {
                fprintf(stderr, "Invalid samples format: %s\n", optarg);
                return 1;
            }
            break;
        case 't':
            if (sscanf(optarg, "%d", &nthreads) != 1) {
                fprintf(stderr, "Invalid thread count format: %s\n", optarg);
                return 1;
            }
            break;
        case 'z':
            if (sscanf(optarg, "%d", &sdl_scale) != 1) {
                fprintf(stderr, "Invalid sdl_scale format: %s\n", optarg);
                return 1;
            }
            break;
        case 'h':
        default:
            fprintf(stderr, help_message, argv[0]);
            return opt == 'h' ? 0 : 1;
        }
    }

    Hittables *world = NULL;
    if (create_scenes(scene, &world) != 0) {
        fprintf(stderr, "Failed to create scene.\n");
        return 1;
    }
    fprintf(stderr, "World has %d objects.\n", hittables_len(world));

    Bvh *bvh = bvh_from_hittables(world); // Consumes the world object
    fprintf(stderr, "Bvh has %d leaves and %d nodes.\n", bvh_count_leaves(bvh),
            bvh_count_nodes(bvh));

    CL_Instance cl_instance;
    if (!load_cl_instance(&cl_instance, "./kernels/main.cl")) {
        fprintf(stderr, "Failed to initialize OpenCL instance.\n");
        return 1;
    }

    Camera camera;
    const double aspect = (double)width / (double)height;
    init_camera(&camera, aspect, samples);

    if (!prepare_kernels_cl(&cl_instance, &camera, width, height, bvh)) {
        fprintf(stderr, "Failed to initialize OpenCL kernels.\n");
        return 1;
    }
    bvh_free(bvh);

    size_t cl_rays_size = width * height * camera.samples_per_pixel;
    CL_Vec3 *cl_colors = (CL_Vec3 *)malloc(cl_rays_size * sizeof(CL_Vec3));
    printf("Starting rendering for %ld rays...\n", cl_rays_size);
    long long start = time_ms();
    if (!raytrace_cl(&cl_instance, cl_colors)) {
        fprintf(stderr, "Failed to run OpenCL kernels.\n");
        return 1;
    }
    long long stop = time_ms();
    printf("Rendering completed in %lld ms\n", stop - start);

    destroy_cl_instance(&cl_instance);

    Vec3 *screen = (Vec3 *)malloc(width * height * sizeof(Vec3));
    for (int i = 0; i < width * height; i++) {
        screen[i].x = (double)cl_colors[i * camera.samples_per_pixel].x;
        screen[i].y = (double)cl_colors[i * camera.samples_per_pixel].y;
        screen[i].z = (double)cl_colors[i * camera.samples_per_pixel].z;
    }

    free(cl_colors);

    if (!init_graphics(width, aspect)) {
        fprintf(stderr, "Failed to initialize graphics.\n");
        return 1;
    }

    render_buffer(screen, width, height);
    free(screen);

    fprintf(stderr, "Done\n");

    stop_graphics();

    return 0;
}
