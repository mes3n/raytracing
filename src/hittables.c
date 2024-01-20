#include "hittables.h"

#include <stdlib.h>

int hit_any(const Hittables *ht, const Ray *ray, const double ray_tmin,
            const double ray_tmax, HitRecord *hit_record) {

    HitRecord tmp_hr;
    double nearest = ray_tmax;
    int any_hit = 0;

    const Hittables *next = ht;

    while (next != NULL) {
        if (next->shape_hit(next->shape, ray, ray_tmin, ray_tmax, &tmp_hr)) {
            if (tmp_hr.t < nearest) {
                *hit_record = tmp_hr;
                nearest = hit_record->t;
                any_hit = 1;
            }
        }
        next = next->next;
    }

    return any_hit;
}

int hittables_add(Hittables **ht, const void *shape, ShapeHitFn shape_hit) {
    if (*ht == NULL) {
        Hittables *new = (Hittables *)malloc(sizeof(Hittables));

        if (new == NULL)
            return 0;

        new->next = NULL;
        new->shape = shape;
        new->shape_hit = shape_hit;

        *ht = new;
        return 1;
    }
    return hittables_add(&((*ht)->next), shape, shape_hit);
}

int hittables_len(Hittables *ht) {
    if (ht == NULL) {
        return 0;
    }
    return 1 + hittables_len(ht->next);
}

void hittables_clear(Hittables *ht) {
    if (ht == NULL)
        return;

    hittables_clear(ht->next);
    free(ht);
}
