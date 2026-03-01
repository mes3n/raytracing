#include "hittables.h"
#include "material.h"

#include "math/bbox.h"
#include "math/interval.h"

#include <stdio.h>
#include <stdlib.h>

bool hit_any(const Hittables *ht, const Ray *ray, const Interval *rayt,
             HitRecord *hit_record, Material **material) {

    HitRecord tmp_hr;
    double nearest = rayt->max;
    bool any_hit = false;

    const Hittables *next = ht;

    while (next != NULL) {
        if (next->shape->hit(next->shape, ray, &(Interval){rayt->min, nearest},
                             &tmp_hr)) {
            *hit_record = tmp_hr;
            *material = (Material *)next->shape->material;
            nearest = hit_record->t;

            any_hit = true;
        }
        next = next->next;
    }

    return any_hit;
}

bool hittables_add(Hittables **ht, const void *shape) {
    if (*ht == NULL) {
        Hittables *new = (Hittables *)malloc(sizeof(Hittables));

        if (new == NULL)
            return 0;

        new->next = NULL;
        new->shape = shape;

        *ht = new;
        return 1;
    }
    return hittables_add(&((*ht)->next), shape);
}

Bbox bbox_from_hittables(const Hittables *hittables) {
    if (hittables->next == NULL)
        return hittables->shape->bbox;
    const Bbox bbox_next = bbox_from_hittables(hittables->next);
    return bbox_combine(&hittables->shape->bbox, &bbox_next);
}

Hittables *hittables_split(Hittables *ht) {
    Hittables *fast = ht, *slow = ht;
    // Split the list int
    while (fast != NULL && fast->next != NULL) {
        fast = fast->next->next;
        if (fast)
            slow = slow->next;
    }
    // Use fast as a temp value to access slow's parent
    fast = slow->next;
    slow->next = NULL;
    return fast;
}

Hittables *hittables_merge(Hittables *left, Hittables *right, const int axis) {
    if (left == NULL)
        return right;
    if (right == NULL)
        return left;
    double left_axis_min = ((Interval *)&left->shape->bbox + axis)->min;
    double right_axis_min = ((Interval *)&right->shape->bbox + axis)->min;
    if (left_axis_min < right_axis_min) {
        left->next = hittables_merge(left->next, right, axis);
        return left;
    }
    right->next = hittables_merge(left, right->next, axis);
    return right;
}

Hittables *hittables_merge_sort(Hittables *ht, const int axis) {
    if (ht == NULL || ht->next == NULL) {
        return ht;
    }

    Hittables *ht_split = hittables_split(ht);

    // Merge sort for both halves
    ht = hittables_merge_sort(ht, axis);
    ht_split = hittables_merge_sort(ht_split, axis);

    return hittables_merge(ht, ht_split, axis);
}

Hittables *hittables_sort(Hittables *ht, const int axis) {
    return hittables_merge_sort(ht, axis);
}

int hittables_len(const Hittables *ht) {
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
