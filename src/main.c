#include "camera.h"
#include "hittables.h"
#include "material.h"
#include "sphere.h"
#include "vec3.h"

#include <stdio.h>

int main(int argc, char **argv) {
    Camera camera;
    if (!init_camera(&camera)) {
        return 1;
    }

    Hittables *world = NULL;

    Lambertian material_center = {
        (ScatterFn)lambertian_scatter,
        .albedo = vec3_from(0.7, 0.3, 0.3),
    };
    Metal material_left = {
        (ScatterFn)metal_scatter,
        .albedo = vec3_from(0.8, 0.8, 0.8),
        .fuzz = 0.3,
    };
    Metal material_right = {
        (ScatterFn)metal_scatter,
        .albedo = vec3_from(0.8, 0.6, 0.2),
        .fuzz = 1.0,
    };
    Lambertian material_ground = {
        (ScatterFn)lambertian_scatter,
        .albedo = vec3_from(0.8, 0.8, 0.0),
    };

    const Sphere sphere_center = (Sphere){(Vec3){0.0, 0.0, -1.0}, .radius = 0.5,
                                          (Material *)&material_center};
    hittables_add(&world, &sphere_center, (ShapeHitFn)sphere_hit);

    const Sphere sphere_left = (Sphere){(Vec3){-1.0, 0.0, -1.0}, .radius = 0.5,
                                        (Material *)&material_left};
    hittables_add(&world, &sphere_left, (ShapeHitFn)sphere_hit);

    const Sphere sphere_right = (Sphere){(Vec3){1.0, 0.0, -1.0}, .radius = 0.5,
                                         (Material *)&material_right};
    hittables_add(&world, &sphere_right, (ShapeHitFn)sphere_hit);

    const Sphere ground = (Sphere){(Vec3){0.0, -100.5, -1.0}, .radius = 100.0,
                                   (Material *)&material_ground};
    hittables_add(&world, &ground, (ShapeHitFn)sphere_hit);

    if (hittables_len(world) != 4) {
        fprintf(stderr, "World has %d objects.\n", hittables_len(world));
        return 1;
    }

    render(&camera, world);

    hittables_clear(world);

    return 0;
}
