#include "camera.h"
#include "ray.h"
#include "vec3.h"

#include "graphics.h"

#include <stdio.h>

int init_camera(Camera *camera) {
    const int image_width = 800u;
    const double aspect_ratio = 16.0 / 9.0;

    const int image_height = (int)((double)image_width / aspect_ratio);

    if (image_height < 1) {
        fprintf(stderr, "Image width cannot be less than 1.\n");
        return -1;
    }

    camera->image_width = image_width;
    camera->image_height = image_height;

    camera->focal_length = 1.0;
    camera->viewport.height = 2.0;
    camera->viewport.width =
        camera->viewport.height * ((double)image_width / (double)image_height);

    camera->viewport.x = vec3_from(camera->viewport.width, 0.0, 0.0);
    camera->viewport.y = vec3_from(0.0, -camera->viewport.height, 0.0);

    camera->viewport.dx =
        vec3_scale(camera->viewport.x, 1.0 / (double)image_width);
    camera->viewport.dy =
        vec3_scale(camera->viewport.y, 1.0 / (double)image_height);

    camera->origin = vec3_from(0.0, 0.0, 0.0);

    camera->viewport.top_left =
        vec3_sub(vec3_sub(vec3_sub(camera->origin,
                                   vec3_from(0.0, 0.0, camera->focal_length)),
                          vec3_scale(camera->viewport.x, 0.5)),
                 vec3_scale(camera->viewport.y, 0.5));
    camera->viewport.pos_at00 = vec3_add(
        camera->viewport.top_left,
        vec3_scale(vec3_add(camera->viewport.dx, camera->viewport.dy), 0.5));

    if (init_graphics(image_width, image_height) < 0) {
        fprintf(stderr, "Failed to initialize graphics.\n");
        return -1;
    }

    return 0;
}

void render(const Camera *camera, const Hittables *world) {
    for (int y = 0; y < camera->image_height; y++) {
        fprintf(stderr, "\rScanlines remaining: %d ", camera->image_height - y);
        fflush(stderr);
        for (int x = 0; x < camera->image_width; x++) {
            Vec3 vp_pixel =
                vec3_add(camera->viewport.pos_at00,
                         vec3_add(vec3_scale(camera->viewport.dx, (double)x),
                                  vec3_scale(camera->viewport.dy, (double)y)));
            Vec3 ray_direction = vec3_sub(vp_pixel, camera->origin);
            Ray ray =
                (Ray){.origin = camera->origin, .direction = ray_direction};
            Vec3 rgb = ray_color(&ray, world);

            set_pixel(x, y, rgb);
        }
    }
    fprintf(stderr, "\rDone.                 \n");

    stop_graphics();
}

Vec3 ray_color(const Ray *ray, const Hittables *world) {
    HitRecord hr;
    if (hit_any(world, ray, &(Interval){0.0, INFINITY}, &hr)) {
        return vec3_scale(
            vec3_from(1.0 + hr.normal.x, 1.0 + hr.normal.y, 1.0 + hr.normal.z),
            0.5);
    }
    Vec3 unit = vec3_normal(ray->direction);
    double s = 0.5 * (unit.y + 1.0);
    return vec3_add(vec3_scale(vec3_from(1.0, 1.0, 1.0), 1.0 - s),
                    vec3_scale(vec3_from(0.5, 0.7, 1.0), s));
}
