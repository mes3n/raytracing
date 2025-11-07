#include "quad.h"
#include "interval.h"
#include "material.h"
#include "vec3.h"

Quad quad_from(Material *material, const Vec3 origin, const Vec3 u,
               const Vec3 v) {
    Quad quad;
    quad.material = material;
    quad.hit = (ShapeHitFn)quad_hit;

    quad.origin = origin;
    quad.u = u;
    quad.v = v;

    Vec3 normal = vec3_cross(u, v);
    quad.normal = vec3_normal(normal);

    quad.d = vec3_dot(quad.normal, quad.origin);
    quad.w = vec3_scale(normal, 1.0 / vec3_dot(normal, normal));

    return quad;
}

bool quad_hit(const Quad *quad, const Ray *ray, const Interval *rayt,
              HitRecord *record) {
    double denom = vec3_dot(quad->normal, ray->direction);
    if (fabs(denom) < 1e-3) {
        return false;
    }
    double t = (quad->d - vec3_dot(quad->normal, ray->origin)) / denom;

    if (!surrounds(rayt, t)) {
        return false;
    }

    Vec3 intersection = ray_at(ray, t);
    Vec3 hit = vec3_sub(intersection, quad->origin);
    double a = vec3_dot(quad->w, vec3_cross(hit, quad->v));
    double b = vec3_dot(quad->w, vec3_cross(quad->u, hit));

    const Interval zero_to_one = {0.0, 1.0};
    if (!surrounds(&zero_to_one, a) || !surrounds(&zero_to_one, b)) {
        return false;
    }

    record->t = t;
    record->point = intersection;
    record->normal = quad->normal;

    set_face_normal(record, ray, quad->normal);

    return true;
}
