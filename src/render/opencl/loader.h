#ifndef CL_LOADER_H
#define CL_LOADER_H

#include <stdbool.h>
#include <stddef.h>

#define RAY_CL_AS_H
#include "kernels/common.h.cl"

bool load_cl_kernel();

CL_Ray *create_rays_cl(CL_Camera *camera, const int width, const int height,
                       size_t *rays_size);

CL_Vec3 *trace_rays_cl(CL_Ray *rays, const size_t rays_size, CL_Bvh *bvhs,
                       const size_t bvhs_size, CL_Hittable *shapes,
                       const size_t shapes_size, CL_Material *materials,
                       const size_t materials_size, const unsigned int depth);

#endif // CL_LOADER_H
