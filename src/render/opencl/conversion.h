#ifndef CONVERSION_H
#define CONVERSION_H

#include "src/render/bvh.h"
#include "src/render/camera.h"
#include <stddef.h>

#define RAY_CL_AS_H
#include "kernels/common.h.cl"

CL_Camera convert_camera(const Camera *camera);

bool convert_bvh(const Bvh *bvh, CL_Bvh *bvh_buffer,
                 CL_Hittable *hittable_buffer, CL_Material *material_buffer);

#endif // CONVERSION_H
