#include "render/camera.h"
#include "render/hittables.h"

#include "graphics.h"
#include "scenes.h"

#include <stdio.h>
#include <unistd.h>

static const char *help_message =
    "Usage: %s\n"
    "Raytracing renderer to create a static image.\n"
    "  -s SCENE    The scene to be rendered\n";

int main(int argc, char **argv) {
    Hittables *world = NULL;

    int scene = 0;

    char opt;
    while ((opt = getopt(argc, argv, "hs:")) != (char)0xff) {
        switch (opt) {
        case 's':
            sscanf(optarg, "%d", &scene);
            break;
        case 'h':
        default:
            fprintf(stderr, help_message, argv[0]);
            return 1;
        }
    }

    if (create_scenes(scene, &world) != 0) {
        fprintf(stderr, "Failed to create scene.\n");
        return 1;
    }
    fprintf(stderr, "World has %d objects.\n", hittables_len(world));

    Camera camera;
    const double aspect = 16.0 / 9.0;
    if (!init_camera(&camera, aspect, 10)) {
        fprintf(stderr, "Failed to create camera.\n");
        return 1;
    }
    if (!init_graphics(350, aspect)) {
        fprintf(stderr, "Failed to initialize graphics.\n");
        return 1;
    }
    render(&camera, world);
    stop_graphics();

    hittables_clear(world);

    return 0;
}
