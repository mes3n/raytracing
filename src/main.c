#include "render/bvh.h"
#include "render/camera.h"
#include "render/hittables.h"
#include "render/opencl/conversion.h"
#include "render/opencl/loader.h"

#include "graphics.h"
#include "scenes.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
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

    if (!load_cl_kernel()) {
        fprintf(stderr, "Failed to initialize OpenCL kernels.\n");
        return 1;
    }

    Camera camera;
    const double aspect = (double)width / (double)height;
    init_camera(&camera, aspect, samples);

    size_t cl_rays_size;
    CL_Camera cl_camera = convert_camera(&camera);
    CL_Ray *cl_rays = create_rays_cl(&cl_camera, width, height, &cl_rays_size);

    CL_Bvh *bvh_buffer = (CL_Bvh *)malloc(bvh_count_nodes(bvh) * sizeof(Bvh));
    const size_t ht_count = bvh_count_leaves(bvh);
    CL_Hittable *hittable_buffer =
        (CL_Hittable *)malloc(ht_count * sizeof(CL_Hittable));
    CL_Material *material_buffer =
        (CL_Material *)malloc(ht_count * sizeof(CL_Material));
    convert_bvh(bvh, bvh_buffer, hittable_buffer, material_buffer);

    size_t idx = 0;
    srand(time(NULL));
    CL_Bvh bvh_test = bvh_buffer[idx];
    printf("Max len: %d\n", bvh_count_nodes(bvh));
    while (bvh_test.hittable_idx == SIZE_MAX) {
        idx = rand() & 0x1 ? bvh_test.right_idx : bvh_test.left_idx;
        bvh_test = bvh_buffer[idx];
        printf("Now at %ld\n", idx);
    }
    CL_Hittable ht_test = hittable_buffer[bvh_test.hittable_idx];
    CL_Material mat_test = material_buffer[ht_test.material_idx];

    printf("shape: %f, %f, %f\n", ht_test.shape.sphere.center.x,
           ht_test.shape.sphere.center.y, ht_test.shape.sphere.center.z);
    printf("mat: 0x%x\n", mat_test.mat_type);

    // for (size_t i = 0; i < cl_rays_size; i++) {
    //     printf("%f, %f, %f\n", cl_rays[i].direction.x,
    //     cl_rays[i].direction.y,
    //            cl_rays[i].direction.z);
    // }

    CL_Vec3 *colors =
        trace_rays_cl(cl_rays, cl_rays_size, bvh_buffer, bvh_count_nodes(bvh),
                      hittable_buffer, ht_count, material_buffer, ht_count,
                      cl_camera.max_depth);

    // for (size_t i = 0; i < cl_rays_size; i++) {
    //     printf("%ld: %f, %f, %f\n\t%f, %f, %f\n", i, cl_rays[i].direction.x,
    //            cl_rays[i].direction.y, cl_rays[i].direction.z, colors[i].x,
    //            colors[i].y, colors[i].z);
    // }

    size_t screen_size = width * height;
    Vec3 *screen = (Vec3 *)malloc(screen_size * sizeof(Vec3));

    for (size_t i = 0; i < screen_size; i++) {
        screen[i] = vec3_zero();
        for (int j = 0; j < camera.samples_per_pixel; j++) {
            size_t k = i * camera.samples_per_pixel + j;
            screen[i] = vec3_add(screen[i], vec3_from((double)colors[k].x,
                                                      (double)colors[k].y,
                                                      (double)colors[k].z));
        }
        screen[i] =
            vec3_scale(screen[i], 1.0 / (double)camera.samples_per_pixel);
    }

    if (!init_graphics(width, aspect)) {
        fprintf(stderr, "Failed to initialize graphics.\n");
        return 1;
    }

    // render(&camera, bvh, nthreads);
    render_buffer(screen, width, height);
    fprintf(stderr, "Done\n");

    stop_graphics();
    bvh_free(bvh);

    return 0;
}
