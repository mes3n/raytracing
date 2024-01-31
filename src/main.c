#include "camera.h"
#include "hittables.h"
#include "sphere.h"
#include "vec3.h"

#include <stdio.h>

int main(int argc, char **argv) {
    Camera camera;
    if (!init_camera(&camera)) {
        return 1;
    }

    Hittables *world = NULL;

    const Sphere sphere_center =
        (Sphere){.center = (Vec3){0.0, 0.0, -1.0},
                 .radius = 0.5,
                 (Scatterer){.scatter = lambertian_scatter,
                             .attenuation = vec3_from(0.7, 0.3, 0.3)}};
    hittables_add(&world, &sphere_center, (ShapeHitFn)sphere_hit);

    const Sphere sphere_left =
        (Sphere){.center = (Vec3){-1.0, 0.0, -1.0},
                 .radius = 0.5,
                 (Scatterer){.scatter = metal_scatter,
                             .attenuation = vec3_from(0.8, 0.8, 0.8)}};
    hittables_add(&world, &sphere_left, (ShapeHitFn)sphere_hit);

    const Sphere sphere_right =
        (Sphere){.center = (Vec3){1.0, 0.0, -1.0},
                 .radius = 0.5,
                 (Scatterer){.scatter = metal_scatter,
                             .attenuation = vec3_from(0.8, 0.6, 0.2)}};
    hittables_add(&world, &sphere_right, (ShapeHitFn)sphere_hit);

    const Sphere ground = (Sphere){
        .center = (Vec3){0.0, -100.5, -1.0},
        .radius = 100.0,
        .scatterer = (Scatterer){.scatter = lambertian_scatter,
                                 .attenuation = vec3_from(0.8, 0.8, 0.0)}};
    hittables_add(&world, &ground, (ShapeHitFn)sphere_hit);

    if (hittables_len(world) != 4) {
        fprintf(stderr, "World has %d objects.\n", hittables_len(world));
        return 1;
    }

    render(&camera, world);

    hittables_clear(world);

    return 0;
}
