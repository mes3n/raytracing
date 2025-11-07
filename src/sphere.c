#include "sphere.h"

#include <math.h>

bool sphere_hit(const Sphere *sphere, const Ray *ray, const Interval *rayt,
                HitRecord *hit_record) {
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

    Vec3 out_n = vec3_scale(vec3_sub(hit_record->point, sphere->center),
                            1.0 / sphere->radius);
    set_face_normal(hit_record, ray, out_n);

    return true;
}
