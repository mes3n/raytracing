#include "camera.h"
#include "vec3.h"

void init_camera(Camera *camera, const int image_width,
                 const int image_height) {
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
}
