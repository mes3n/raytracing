#include "sphere.h"

#include <math.h>

void set_face_normal(HitRecord *hit_record, const Ray *ray, const Vec3 out_n) {
    hit_record->front_face = vec3_dot(ray->direction, out_n) < 0;
    hit_record->normal =
        hit_record->front_face ? out_n : vec3_scale(out_n, -1.0);
}

bool sphere_hit(const Sphere *sphere, const Ray *ray, const Interval *rayt,
                HitRecord *hit_record, Material **material) {
    Vec3 oc = vec3_sub(ray->origin, sphere->center);
    double a = vec3_length_squared(ray->direction);
    double b_2 = vec3_dot(oc, ray->direction);
    double c = vec3_length_squared(oc) - sphere->radius * sphere->radius;
    double d = b_2 * b_2 - a * c;

    if (d < 0.0)
        return false;
    double sqd = sqrt(d);

    double r = (-b_2 - sqd) / a;
    if (!surrounds(rayt, r)) {
        r = (-b_2 + sqd) / a;
        if (!surrounds(rayt, r))
            return false;
    }

    hit_record->t = r;
    hit_record->point = ray_at(ray, hit_record->t);
    hit_record->normal = vec3_scale(vec3_sub(hit_record->point, sphere->center),
                                    1.0 / sphere->radius);
    *material = sphere->material;

    Vec3 out_n = vec3_scale(vec3_sub(hit_record->point, sphere->center),
                            1.0 / sphere->radius);
    set_face_normal(hit_record, ray, out_n);

    return true;
}
