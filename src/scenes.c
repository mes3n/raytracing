#include "scenes.h"

#include "hittables.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "vec3.h"

#include <stdlib.h>
#include <sys/types.h>

static inline void many_spheres(Hittables **world) {
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
                *sphere = (Sphere){mat, (ShapeHitFn)sphere_hit, center,
                                   .radius = 0.2};
                hittables_add(world, sphere);
            }
        }
    }
}

static inline void default_scene(Hittables **world) {
    Lambertian *material_ground = (Lambertian *)malloc(sizeof(Lambertian));
    *material_ground = (Lambertian){
        (ScatterFn)lambertian_scatter,
        .albedo = vec3_from(0.5, 0.5, 0.5),
    };
    Lambertian *material_left = (Lambertian *)malloc(sizeof(Lambertian));
    *material_left = (Lambertian){
        (ScatterFn)lambertian_scatter,
        .albedo = vec3_from(0.4, 0.2, 0.1),
    };
    Dielectric *material_center = (Dielectric *)malloc(sizeof(Dielectric));
    *material_center = (Dielectric){
        (ScatterFn)dielectric_scatter,
        .refraction = 1.5,
    };
    Metal *material_right = (Metal *)malloc(sizeof(Metal));
    *material_right = (Metal){
        (ScatterFn)metal_scatter,
        .albedo = vec3_from(0.7, 0.6, 0.5),
        .fuzz = 0.0,
    };

    Sphere *ground = (Sphere *)malloc(sizeof(Sphere));
    *ground = (Sphere){
        (Material *)material_ground,
        (ShapeHitFn)sphere_hit,
        vec3_from(0.0, -1000.0, 0.0),
        .radius = 1000.0,
    };

    Sphere *sphere_left = (Sphere *)malloc(sizeof(Sphere));
    *sphere_left = (Sphere){(Material *)material_left, (ShapeHitFn)sphere_hit,
                            vec3_from(-4.0, 1.0, 0.0), .radius = 1.0};

    Sphere *sphere_center = (Sphere *)malloc(sizeof(Sphere));
    *sphere_center =
        (Sphere){(Material *)material_center, (ShapeHitFn)sphere_hit,
                 vec3_from(0.0, 1.0, 0.0), .radius = 1.0};

    Sphere *sphere_right = (Sphere *)malloc(sizeof(Sphere));
    *sphere_right = (Sphere){(Material *)material_right, (ShapeHitFn)sphere_hit,
                             vec3_from(4.0, 1.0, 0.0), .radius = 1.0};

    hittables_add(world, ground);
    hittables_add(world, sphere_left);
    hittables_add(world, sphere_center);
    hittables_add(world, sphere_right);
}

static inline void quads(Hittables **world) {
    Lambertian *left_red = (Lambertian *)malloc(sizeof(Lambertian));
    *left_red = (Lambertian){(ScatterFn)lambertian_scatter,
                             .albedo = vec3_from(1.0, 0.2, 0.2)};
    Lambertian *back_green = (Lambertian *)malloc(sizeof(Lambertian));
    *back_green = (Lambertian){(ScatterFn)lambertian_scatter,
                               .albedo = vec3_from(0.2, 1.0, 0.2)};
    Lambertian *right_blue = (Lambertian *)malloc(sizeof(Lambertian));
    *right_blue = (Lambertian){(ScatterFn)lambertian_scatter,
                               .albedo = vec3_from(0.2, 0.2, 1.0)};
    Lambertian *upper_orange = (Lambertian *)malloc(sizeof(Lambertian));
    *upper_orange = (Lambertian){(ScatterFn)lambertian_scatter,
                                 .albedo = vec3_from(1.0, 0.5, 0.0)};
    Lambertian *lower_teal = (Lambertian *)malloc(sizeof(Lambertian));
    *lower_teal = (Lambertian){(ScatterFn)lambertian_scatter,
                               .albedo = vec3_from(0.2, 0.8, 0.8)};

    Quad *quad_left = (Quad *)malloc(sizeof(Quad));
    *quad_left = quad_from((Material *)left_red, vec3_from(-3.0, -2.0, 5.0),
                           vec3_from(0.0, 0.0, -4.0), vec3_from(0.0, 4.0, 0.0));

    Quad *quad_back = (Quad *)malloc(sizeof(Quad));
    *quad_back = quad_from((Material *)back_green, vec3_from(-2.0, -2.0, 0.0),
                           vec3_from(4.0, 0.0, 0.0), vec3_from(0.0, 4.0, 0.0));

    Quad *quad_right = (Quad *)malloc(sizeof(Quad));
    *quad_right = quad_from((Material *)right_blue, vec3_from(3.0, -2.0, 1.0),
                            vec3_from(0.0, 0.0, 4.0), vec3_from(0.0, 4.0, 0.0));

    Quad *quad_upper = (Quad *)malloc(sizeof(Quad));
    *quad_upper = quad_from((Material *)upper_orange, vec3_from(-2.0, 3.0, 1.0),
                            vec3_from(4.0, 0.0, 0.0), vec3_from(0.0, 0.0, 4.0));

    Quad *quad_lower = (Quad *)malloc(sizeof(Quad));
    *quad_lower =
        quad_from((Material *)lower_teal, vec3_from(-2.0, -3.0, 5.0),
                  vec3_from(4.0, 0.0, 0.0), vec3_from(0.0, 0.0, -4.0));

    hittables_add(world, quad_left);
    hittables_add(world, quad_back);
    hittables_add(world, quad_right);
    hittables_add(world, quad_upper);
    hittables_add(world, quad_lower);
}

int create_scenes(const int scene, Hittables **world) {
    switch (scene) {
    case 0:
        default_scene(world);
        break;
    case 1:
        default_scene(world);
        many_spheres(world);
        break;
    case 2:
        quads(world);
        break;
    default:
        return 1;
    }
    return 0;
}
