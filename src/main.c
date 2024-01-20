#include "camera.h"
#include "hittables.h"
#include "sphere.h"

int main(int argc, char **argv) {
    Camera camera;
    if (init_camera(&camera) < 0) {
        return 1;
    }

    Hittables *world = NULL;

    const Sphere sphere =
        (Sphere){.center = (Vec3){0.0, 0.0, -1.0}, .radius = 0.5};
    hittables_add(&world, &sphere, (ShapeHitFn)sphere_hit);

    const Sphere sphere2 =
        (Sphere){.center = (Vec3){0.0, 0.5, -1.0}, .radius = 0.5};
    hittables_add(&world, &sphere2, (ShapeHitFn)sphere_hit);

    const Sphere ground =
        (Sphere){.center = (Vec3){0.0, -100.5, -1.0}, .radius = 100.0};
    hittables_add(&world, &ground, (ShapeHitFn)sphere_hit);

    if (hittables_len(world) != 3) {
        // fprintf(stderr, "World has %d objects.\n", hittables_len(world));
        return 1;
    }

    render(&camera, world);

    hittables_clear(world);

    return 0;
}
