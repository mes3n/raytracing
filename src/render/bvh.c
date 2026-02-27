#include "bvh.h"
#include "bbox.h"
#include "hittables.h"
#include "interval.h"
#include "material.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

Bvh *bvh_from_hittables(Hittables *hittables) {
    if (hittables == NULL)
        return NULL;
    Bvh *root = (Bvh *)malloc(sizeof(Bvh));
    *root = (Bvh){0};
    if (hittables->next == NULL) {
        root->shape = hittables->shape;
        root->bbox = root->shape->bbox;
        hittables_clear(hittables); // Consume the hittables object
        return root;
    }
    root->bbox = bbox_from_hittables(hittables);
    hittables = hittables_sort(hittables, bbox_max_axis(&root->bbox));
    Hittables *ht_split = hittables_split(hittables);
    root->left = bvh_from_hittables(hittables);
    root->right = bvh_from_hittables(ht_split);
    assert(root->left != NULL);
    assert(root->right != NULL);

    return root;
}

void bvh_free(Bvh *bvh) {
    if (bvh->shape == NULL) {
        bvh_free(bvh->left);
        bvh_free(bvh->right);
    }
    free(bvh);
}

int bvh_count_leaves(const Bvh *bvh) {
    if (bvh->shape != NULL)
        return 1;
    return bvh_count_leaves(bvh->left) + bvh_count_leaves(bvh->right);
}

bool bvh_hit(const Bvh *bvh, const Ray *ray, const Interval *rayt,
             HitRecord *hit_record, Material **material) {
    if (!bbox_hit(&bvh->bbox, ray, *rayt))
        return false;

    if (bvh->shape != NULL) {
        bool shape_hit = bvh->shape->hit(bvh->shape, ray, rayt, hit_record);
        if (shape_hit)
            *material = (Material *)bvh->shape->material;
        return shape_hit;
    }

    bool hit_left = bvh_hit(bvh->left, ray, rayt, hit_record, material);
    const Interval interval =
        interval_from(rayt->min, hit_left ? hit_record->t : rayt->max);
    bool hit_right = bvh_hit(bvh->right, ray, &interval, hit_record, material);

    return hit_left || hit_right;
}
