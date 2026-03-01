#include "scenes.h"

#include "render/hittables.h"
#include "render/material.h"
#include "render/shapes/quad.h"
#include "render/shapes/sphere.h"
#include "render/texture.h"

#include "render/math/vec3.h"

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
                    mat = (Material *)new_lambertian_solid(albedo);
                } else if (choose_mat < 0.95) {
                    Vec3 albedo = vec3_random_range(0.5, 1.0);
                    double fuzz = random_double_range(0.0, 0.5);
                    mat = (Material *)new_metal(albedo, fuzz);
                } else {
                    mat = (Material *)new_dielectric(1.5);
                }
                double bounce = random_double();
                Sphere *sphere = new_sphere(
                    mat, center,
                    vec3_from(0.0, bounce < 0.5 ? bounce : 0.0, 0.0), 0.2);
                hittables_add(world, sphere);
            }
        }
    }
}

static inline void default_scene(Hittables **world) {
    Lambertian *material_ground = new_lambertian((Texture *)new_checker_texture(
        (Texture *)new_solid_texture(vec3_from(0.2, 0.3, 0.1)),
        (Texture *)new_solid_texture(vec3_from(0.9, 0.9, 0.9))));

    Lambertian *material_left = new_lambertian_solid(vec3_from(0.4, 0.2, 0.1));
    Dielectric *material_center = new_dielectric(1.5);
    Metal *material_right = new_metal(vec3_from(0.7, 0.6, 0.5), 0.0);

    Sphere *ground =
        new_sphere((Material *)material_ground, vec3_from(0.0, -1000.0, 0.0),
                   vec3_zero(), 1000.0);

    Sphere *sphere_left = new_sphere(
        (Material *)material_left, vec3_from(-4.0, 1.0, 0.0), vec3_zero(), 1.0);

    Sphere *sphere_center =
        new_sphere((Material *)material_center, vec3_from(0.0, 1.0, 0.0),
                   vec3_zero(), 1.0);

    Sphere *sphere_right = new_sphere(
        (Material *)material_right, vec3_from(4.0, 1.0, 0.0), vec3_zero(), 1.0);

    hittables_add(world, ground);
    hittables_add(world, sphere_left);
    hittables_add(world, sphere_center);
    hittables_add(world, sphere_right);
}

static inline void checkered_spheres(Hittables **world) {
    Lambertian *checker_texture = new_lambertian((Texture *)new_checker_texture(
        (Texture *)new_solid_texture(vec3_from(0.2, 0.3, 0.1)),
        (Texture *)new_solid_texture(vec3_from(0.9, 0.9, 0.9))));

    hittables_add(world,
                  new_sphere((Material *)checker_texture,
                             vec3_from(0.0, -10.0, 0.0), vec3_zero(), 10.0));
    hittables_add(world,
                  new_sphere((Material *)checker_texture,
                             vec3_from(0.0, 10.0, 0.0), vec3_zero(), 10.0));
}

static inline void quads(Hittables **world) {
    Lambertian *left_red = new_lambertian_solid(vec3_from(1.0, 0.2, 0.2));
    Lambertian *back_green = new_lambertian_solid(vec3_from(0.2, 1.0, 0.2));
    Lambertian *right_blue = new_lambertian_solid(vec3_from(0.2, 0.2, 1.0));
    Lambertian *upper_orange = new_lambertian_solid(vec3_from(1.0, 0.5, 0.0));
    Lambertian *lower_teal = new_lambertian_solid(vec3_from(0.2, 0.8, 0.8));

    Quad *quad_left =
        new_quad((Material *)left_red, vec3_from(-3.0, -2.0, 5.0),
                 vec3_from(0.0, 0.0, -4.0), vec3_from(0.0, 4.0, 0.0));

    Quad *quad_back =
        new_quad((Material *)back_green, vec3_from(-2.0, -2.0, 0.0),
                 vec3_from(4.0, 0.0, 0.0), vec3_from(0.0, 4.0, 0.0));

    Quad *quad_right =
        new_quad((Material *)right_blue, vec3_from(3.0, -2.0, 1.0),
                 vec3_from(0.0, 0.0, 4.0), vec3_from(0.0, 4.0, 0.0));

    Quad *quad_upper =
        new_quad((Material *)upper_orange, vec3_from(-2.0, 3.0, 1.0),
                 vec3_from(4.0, 0.0, 0.0), vec3_from(0.0, 0.0, 4.0));

    Quad *quad_lower =
        new_quad((Material *)lower_teal, vec3_from(-2.0, -3.0, 5.0),
                 vec3_from(4.0, 0.0, 0.0), vec3_from(0.0, 0.0, -4.0));

    hittables_add(world, quad_left);
    hittables_add(world, quad_back);
    hittables_add(world, quad_right);
    hittables_add(world, quad_upper);
    hittables_add(world, quad_lower);
}

void earth(Hittables **world) {
    Lambertian *earth_material =
        new_lambertian((Texture *)new_image_texture("assets/earthmap.jpg"));
    Sphere *sphere = new_sphere((Material *)earth_material,
                                vec3_from(0.0, 0.0, 0.0), vec3_zero(), 2.0);
    hittables_add(world, sphere);
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
    case 3:
        checkered_spheres(world);
        break;
    case 4:
        earth(world);
        break;
    default:
        return 1;
    }
    return 0;
}
