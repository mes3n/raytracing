#include "camera.h"
#include "hittables.h"
#include "material.h"
#include "sphere.h"
#include "vec3.h"

#include <stdio.h>
#include <stdlib.h>

int main(/* int argc, char **argv */) {
    Hittables *world = NULL;

    Lambertian material_ground = {
        (ScatterFn)lambertian_scatter,
        .albedo = vec3_from(0.5, 0.5, 0.5),
    };
    const Sphere ground =
        (Sphere){vec3_from(0.0, -1000.0, 0.0), .radius = 1000.0,
                 (Material *)&material_ground};
    hittables_add(&world, &ground, (ShapeHitFn)sphere_hit);

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            Vec3 center = vec3_from(a + 0.9 * random_double(), 0.2,
                                    b + 0.9 * random_double());

            if (vec3_length(vec3_sub(center, vec3_from(4.0, 0.2, 0.0))) > 0.9) {
                Material *mat;
                if (choose_mat < 0.8) {
                    Vec3 albedo =
                        vec3_scale_from_vec3(vec3_random(), vec3_random());
                    Lambertian *material =
                        (Lambertian *)malloc(sizeof(Lambertian));
                    *material = (Lambertian){(ScatterFn)lambertian_scatter,
                                             .albedo = albedo};
                    mat = (Material *)material;
                } else if (choose_mat < 0.95) {
                    Vec3 albedo = vec3_random_range(0.5, 1.0);
                    double fuzz = random_double_range(0.0, 0.5);
                    Metal *material = (Metal *)malloc(sizeof(Metal));
                    *material = (Metal){(ScatterFn)metal_scatter,
                                        .albedo = albedo, .fuzz = fuzz};
                    mat = (Material *)material;
                } else {
                    Dielectric *material =
                        (Dielectric *)malloc(sizeof(Dielectric));
                    *material = (Dielectric){(ScatterFn)dielectric_scatter,
                                             .refraction = 1.5};
                    mat = (Material *)material;
                }
                Sphere *sphere = (Sphere *)malloc(sizeof(Sphere));
                *sphere = (Sphere){center, .radius = 0.2, mat};
                hittables_add(&world, sphere, (ShapeHitFn)sphere_hit);
            }
        }
    }

    Lambertian material_left = {
        (ScatterFn)lambertian_scatter,
        .albedo = vec3_from(0.4, 0.2, 0.1),
    };
    const Sphere sphere_left = (Sphere){
        vec3_from(-4.0, 1.0, 0.0), .radius = 1.0, (Material *)&material_left};
    hittables_add(&world, &sphere_left, (ShapeHitFn)sphere_hit);

    Dielectric material_center = {
        (ScatterFn)dielectric_scatter,
        .refraction = 1.5,
    };
    const Sphere sphere_center = (Sphere){
        vec3_from(0.0, 1.0, 0.0), .radius = 1.0, (Material *)&material_center};
    hittables_add(&world, &sphere_center, (ShapeHitFn)sphere_hit);

    Metal material_right = {
        (ScatterFn)metal_scatter,
        .albedo = vec3_from(0.7, 0.6, 0.5),
        .fuzz = 0.0,
    };
    const Sphere sphere_right = (Sphere){
        vec3_from(4.0, 1.0, 0.0), .radius = 1.0, (Material *)&material_right};
    hittables_add(&world, &sphere_right, (ShapeHitFn)sphere_hit);

    fprintf(stderr, "World has %d objects.\n", hittables_len(world));
    // if (hittables_len(world) != 5) {
    //     fprintf(stderr, "World has %d objects.\n", hittables_len(world));
    //     return 1;
    // }

    Camera camera;
    if (!init_camera(&camera)) {
        return 1;
    }
    render(&camera, world);

    hittables_clear(world);

    return 0;
}
