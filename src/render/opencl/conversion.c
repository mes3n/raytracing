#include "conversion.h"

#include "kernels/common.h.cl"
#include "src/render/bvh.h"
#include "src/render/material.h"
#include "src/render/shapes/quad.h"
#include "src/render/shapes/sphere.h"
#include "src/render/texture.h"

#include <stddef.h>
#include <stdint.h>

CL_Camera convert_camera(const Camera *camera) {
    CL_Camera cl_camera;

    cl_camera.samples_per_pixel = (cl_int)camera->samples_per_pixel;
    cl_camera.max_depth = (cl_int)camera->max_depth;

    cl_camera.origin.x = (cl_float)camera->origin.x;
    cl_camera.origin.y = (cl_float)camera->origin.y;
    cl_camera.origin.z = (cl_float)camera->origin.z;
    cl_camera.origin.w = (cl_float)0.0f;

    cl_camera.defocus_angle = (cl_float)camera->defocus_angle;

    cl_camera.defocus_disc_u.x = (cl_float)camera->defocus_disc_u.x;
    cl_camera.defocus_disc_u.y = (cl_float)camera->defocus_disc_u.y;
    cl_camera.defocus_disc_u.z = (cl_float)camera->defocus_disc_u.z;
    cl_camera.defocus_disc_u.w = (cl_float)0.0f;

    cl_camera.defocus_disc_v.x = (cl_float)camera->defocus_disc_v.x;
    cl_camera.defocus_disc_v.y = (cl_float)camera->defocus_disc_v.y;
    cl_camera.defocus_disc_v.z = (cl_float)camera->defocus_disc_v.z;
    cl_camera.defocus_disc_v.w = (cl_float)0.0f;

    cl_camera.viewport.u.x = (cl_float)camera->viewport.u.x;
    cl_camera.viewport.u.y = (cl_float)camera->viewport.u.y;
    cl_camera.viewport.u.z = (cl_float)camera->viewport.u.z;
    cl_camera.viewport.u.w = (cl_float)0.0f;

    cl_camera.viewport.v.x = (cl_float)camera->viewport.v.x;
    cl_camera.viewport.v.y = (cl_float)camera->viewport.v.y;
    cl_camera.viewport.v.z = (cl_float)camera->viewport.v.z;
    cl_camera.viewport.v.w = (cl_float)0.0f;

    cl_camera.viewport.pos_at00.x = (cl_float)camera->viewport.pos_at00.x;
    cl_camera.viewport.pos_at00.y = (cl_float)camera->viewport.pos_at00.y;
    cl_camera.viewport.pos_at00.z = (cl_float)camera->viewport.pos_at00.z;
    cl_camera.viewport.pos_at00.w = (cl_float)0.0f;

    return cl_camera;
}

CL_Material convert_material(const Material *material) {
    CL_Material cl_material;

    if ((long)material->scatter == (long)lambertian_scatter) {
        cl_material.mat_type = LAMBERTIAN;
        const Lambertian *lambertian = (Lambertian *)material;
        cl_material.mat.lambertian.albedo.x =
            (cl_float)((SolidTexture *)lambertian->texture)->albedo.x;
        cl_material.mat.lambertian.albedo.y =
            (cl_float)((SolidTexture *)lambertian->texture)->albedo.y;
        cl_material.mat.lambertian.albedo.z =
            (cl_float)((SolidTexture *)lambertian->texture)->albedo.z;
        cl_material.mat.lambertian.albedo.w = (cl_float)0.0f;
    } else if ((long)material->scatter == (long)metal_scatter) {
        cl_material.mat_type = METAL;
        const Metal *metal = (Metal *)material;
        cl_material.mat.metal.albedo.x = (cl_float)metal->albedo.x;
        cl_material.mat.metal.albedo.y = (cl_float)metal->albedo.y;
        cl_material.mat.metal.albedo.z = (cl_float)metal->albedo.z;
        cl_material.mat.metal.fuzz = (cl_float)metal->fuzz;
    } else if ((long)material->scatter == (long)dielectric_scatter) {
        cl_material.mat_type = DIELECTRIC;
        const Dielectric *dielectric = (Dielectric *)material;
        cl_material.mat.dielectric.refraction =
            (cl_float)dielectric->refraction;
    } else {
        // WARN: Error
    }

    return cl_material;
}

CL_Hittable convert_hittable(const Hittable *hittable, const size_t mat_idx) {
    CL_Hittable cl_hittable;

    cl_hittable.material_idx = mat_idx;

    cl_hittable.bbox.min.x = (cl_float)hittable->bbox.x.min;
    cl_hittable.bbox.max.x = (cl_float)hittable->bbox.x.max;
    cl_hittable.bbox.min.y = (cl_float)hittable->bbox.y.min;
    cl_hittable.bbox.max.y = (cl_float)hittable->bbox.y.max;
    cl_hittable.bbox.min.z = (cl_float)hittable->bbox.z.min;
    cl_hittable.bbox.max.z = (cl_float)hittable->bbox.z.max;

    if ((long)hittable->hit == (long)sphere_hit) {
        cl_hittable.shape_type = SPHERE;
        const Sphere *sphere = (Sphere *)hittable;
        cl_hittable.shape.sphere.center.x = (cl_float)sphere->center.x;
        cl_hittable.shape.sphere.center.y = (cl_float)sphere->center.y;
        cl_hittable.shape.sphere.center.z = (cl_float)sphere->center.z;
        cl_hittable.shape.sphere.center.w = (cl_float)0.0f;
        cl_hittable.shape.sphere.dcenter.x = (cl_float)sphere->dcenter.x;
        cl_hittable.shape.sphere.dcenter.y = (cl_float)sphere->dcenter.y;
        cl_hittable.shape.sphere.dcenter.z = (cl_float)sphere->dcenter.z;
        cl_hittable.shape.sphere.dcenter.w = (cl_float)0.0f;
        cl_hittable.shape.sphere.radius = (cl_float)sphere->radius;
    } else if ((long)hittable->hit == (long)quad_hit) {
        // TODO: sooon
    } else {
        // WARN: Error
    }

    return cl_hittable;
}

CL_Bvh convert_bvh_node(const Bvh *bvh_node, CL_Bvh *bvh_buffer, size_t *ib,
                        CL_Hittable *hittable_buffer, size_t *ih,
                        CL_Material *material_buffer, size_t *im) {
    CL_Bvh cl_bvh_node;

    cl_bvh_node.bbox.min.x = (cl_float)bvh_node->bbox.x.min;
    cl_bvh_node.bbox.max.x = (cl_float)bvh_node->bbox.x.max;
    cl_bvh_node.bbox.min.y = (cl_float)bvh_node->bbox.y.min;
    cl_bvh_node.bbox.max.y = (cl_float)bvh_node->bbox.y.max;
    cl_bvh_node.bbox.min.z = (cl_float)bvh_node->bbox.z.min;
    cl_bvh_node.bbox.max.z = (cl_float)bvh_node->bbox.z.max;

    if (bvh_node->shape != NULL) {
        cl_bvh_node.left_idx = SIZE_MAX;
        cl_bvh_node.right_idx = SIZE_MAX;
        cl_bvh_node.hittable_idx = (*ih)++;

        hittable_buffer[cl_bvh_node.hittable_idx] =
            convert_hittable(bvh_node->shape, (*im)++);

        material_buffer[hittable_buffer[cl_bvh_node.hittable_idx]
                            .material_idx] =
            convert_material(bvh_node->shape->material);
    } else {
        cl_bvh_node.hittable_idx = SIZE_MAX;
        cl_bvh_node.left_idx = (*ib)++;
        cl_bvh_node.right_idx = (*ib)++;

        bvh_buffer[cl_bvh_node.left_idx] =
            convert_bvh_node(bvh_node->left, bvh_buffer, ib, hittable_buffer,
                             ih, material_buffer, im);
        bvh_buffer[cl_bvh_node.right_idx] =
            convert_bvh_node(bvh_node->right, bvh_buffer, ib, hittable_buffer,
                             ih, material_buffer, im);
    }

    return cl_bvh_node;
}

bool convert_bvh(const Bvh *bvh, CL_Bvh *bvh_buffer,
                 CL_Hittable *hittable_buffer, CL_Material *material_buffer) {

    size_t bvh_idx = 1; // 0 is preserved for root node
    size_t ht_idx = 0;
    size_t mat_idx = 0;
    bvh_buffer[0] = convert_bvh_node(bvh, bvh_buffer, &bvh_idx, hittable_buffer,
                                     &ht_idx, material_buffer, &mat_idx);

    return true;
}
