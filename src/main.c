#include "camera.h"
#include "graphics.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"

#include <stdio.h>

const int image_width = 400u;
const double aspect_ratio = 16.0 / 9.0;

const int image_height = (int)((double)image_width / aspect_ratio);

const Sphere sphere = (Sphere){.center = (Vec3){0.0, 0.0, -1.0}, .radius = 0.5};

Vec3 ray_color(const Ray *ray) {
    HitRecord hr;
    if (sphere_hit(&sphere, ray, 0.0, 100.0, &hr)) {
        return vec3_scale(
            vec3_from(1.0 + hr.normal.x, 1.0 + hr.normal.y, 1.0 + hr.normal.z),
            0.5);
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

    if (init_graphics(image_width, image_height) < 0) {
        fprintf(stderr, "Failed to initialize graphics.\n");
        return 1;
    }

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

            set_pixel(x, y, rgb);
        }
    }
    fprintf(stderr, "\rDone.                 \n");

    stop_graphics();

    return 0;
}
