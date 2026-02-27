#include "sphere.h"
#include "../interval.h"
#include "../material.h"
#include "../vec3.h"

#include <math.h>

Sphere sphere_from(const Material *material, const Vec3 center,
                   const Vec3 dcenter, const double radius) {
    Sphere sphere = {0};
    sphere.material = material;
    sphere.hit = (ShapeHitFn)sphere_hit;
    sphere.center = center;
    sphere.dcenter = dcenter;
    sphere.radius = radius;

    sphere.bbox = sphere_bbox(&sphere);
    return sphere;
}

bool sphere_hit(const Sphere *sphere, const Ray *ray, const Interval *rayt,
                HitRecord *hit_record) {
    Vec3 center =
        vec3_add(sphere->center, vec3_scale(sphere->dcenter, ray->time));
    Vec3 oc = vec3_sub(ray->origin, center);
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
    hit_record->normal =
        vec3_scale(vec3_sub(hit_record->point, center), 1.0 / sphere->radius);

    Vec3 out_n =
        vec3_scale(vec3_sub(hit_record->point, center), 1.0 / sphere->radius);
    set_face_normal(hit_record, ray, out_n);

    return true;
}

Bbox sphere_bbox(const Sphere *sphere) {
    const double r = sphere->radius;
    const Vec3 rv = vec3_from(r, r, r);
    const Vec3 center0 =
        vec3_add(sphere->center, vec3_scale(sphere->dcenter, 0.0));
    const Vec3 center1 =
        vec3_add(sphere->center, vec3_scale(sphere->dcenter, 1.0));
    const Bbox bbox0 =
        bbox_from_positions(vec3_sub(center0, rv), vec3_add(center0, rv));
    const Bbox bbox1 =
        bbox_from_positions(vec3_sub(center1, rv), vec3_add(center1, rv));
    return bbox_combine(&bbox0, &bbox1);
}
