#include "camera.h"
#include "graphics.h"
#include "ray.h"
#include "vec3.h"

#include <math.h>
#include <stdio.h>

const int image_width = 400u;
const double aspect_ratio = 16.0 / 9.0;

const int image_height = (int)((double)image_width / aspect_ratio);

double hit_sphere(const Vec3 center, const double radius, const Ray *ray) {
    Vec3 oc = vec3_sub(ray->origin, center);
    double a = vec3_length_squared(ray->direction);
    double b_2 = vec3_dot(oc, ray->direction);
    double c = vec3_length_squared(oc) - radius * radius;
    double d = b_2 * b_2 - a * c;

    if (d < 0.0) {
        return -1.0;
    } else {
        return (-b_2 - sqrt(d)) / a;
    }
}

Vec3 ray_color(const Ray *ray) {
    double t = hit_sphere(vec3_from(0.0, 0.0, -1.0), 0.5, ray);
    if (t > 0.0) {
        Vec3 n = vec3_normal(vec3_sub(ray_at(ray, t), vec3_from(0.0, 0.0, -1.0)));
        return vec3_scale(vec3_from(1.0 + n.x, 1.0 + n.y, 1.0 + n.z), 0.5);
    }
    Vec3 unit = vec3_normal(ray->direction);
    double s = 0.5 * (unit.y + 1.0);
    return vec3_add(vec3_scale(vec3_from(1.0, 1.0, 1.0), 1.0 - s),
                    vec3_scale(vec3_from(0.5, 0.7, 1.0), s));
}

int main(int argc, char **argv) {
    if (image_height < 1) {
        fprintf(stderr, "Image width cannot be less than 1.\n");
        return 1;
    }

    init_graphics(image_width, image_height);

    Camera camera;
    init_camera(&camera, image_width, image_height);

    for (int y = 0; y < image_height; y++) {
        fprintf(stderr, "\rScanlines remaining: %d ", image_height - y);
        fflush(stderr);
        for (int x = 0; x < image_width; x++) {
            Vec3 vp_pixel =
                vec3_add(camera.viewport.pos_at00,
                         vec3_add(vec3_scale(camera.viewport.dx, (double)x),
                                  vec3_scale(camera.viewport.dy, (double)y)));
            Vec3 ray_direction = vec3_sub(vp_pixel, camera.origin);
            Ray ray =
                (Ray){.origin = camera.origin, .direction = ray_direction};
            Vec3 rgb = ray_color(&ray);

#ifndef MAKE_PPM
            set_pixel(x, y, rgb);
#else
            set_pixel(rgb);
#endif
        }
    }
    fprintf(stderr, "\rDone.                 \n");

    return 0;
}
