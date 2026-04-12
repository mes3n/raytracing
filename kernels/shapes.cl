#pragma once

#include "common.h.cl"
#include "math.cl"

bool quad_hit(const struct Quad *quad, const Ray *ray, const Interval *rayt,
              HitRecord *record) {
    float denom = dot(quad->normal, ray->direction);
    if (fabs(denom) < 1e-3) {
        return false;
    }
    float t = (quad->d - dot(quad->normal, ray->origin)) / denom;

    if (!surrounds(rayt, t)) {
        return false;
    }

    float3 intersection = ray_at(ray, t);
    float3 hit = intersection - quad->origin;
    float a = dot(quad->w, cross(hit, quad->v));
    float b = dot(quad->w, cross(quad->u, hit));

    const Interval zero_to_one = {0, 1};
    if (!surrounds(&zero_to_one, a) || !surrounds(&zero_to_one, b)) {
        return false;
    }

    record->t = t;
    record->point = intersection;
    record->normal = quad->normal;

    set_face_normal(record, ray, quad->normal);

    return true;
}

bool sphere_hit(const struct Sphere *sphere, const Ray *ray,
                const Interval *rayt, HitRecord *hit_record) {
    float3 center = sphere->center + sphere->dcenter * ray->time;
    float3 oc = ray->origin - center;
    float a = dot(ray->direction, ray->direction);
    float b_2 = dot(oc, ray->direction);
    float c = dot(oc, oc) - sphere->radius * sphere->radius;
    float d = b_2 * b_2 - a * c;

    if (d < 0.0)
        return false;
    float sqd = sqrt(d);

    float r = (-b_2 - sqd) / a;
    if (!surrounds(rayt, r)) {
        r = (-b_2 + sqd) / a;
        if (!surrounds(rayt, r))
            return false;
    }

    hit_record->t = r;
    hit_record->point = ray_at(ray, hit_record->t);
    hit_record->normal = hit_record->point - center * (1 / sphere->radius);

    float3 out_n = hit_record->normal;
    set_face_normal(hit_record, ray, out_n);

    const float theta = acos(-out_n.y);
    const float phi = atan2(-out_n.z, out_n.x) + M_PI;

    hit_record->u = phi / (2 * M_PI);
    hit_record->v = theta / M_PI;

    return true;
}
