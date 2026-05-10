#ifndef CL_LOADER_H
#define CL_LOADER_H

#include "src/render/bvh.h"
#include "src/render/camera.h"
#include <stdbool.h>
#include <stddef.h>

#define RAY_CL_AS_H
#include "kernels/common.h.cl"

#define CREATE_RAYS_KERNEL 0
#define TRACE_RAYS_KERNEL 1
#define AVERAGE_SAMPLES_KERNEL 2

typedef struct {
    cl_kernel id;
    cl_uint work_dim;
    size_t work_size[3];
} CL_Kernel;

typedef struct {
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;

    cl_mem camera_buffer;
    cl_mem rays_buffer_rw;

    cl_mem bvhs_buffer;
    cl_mem shapes_buffer;
    cl_mem materials_buffer;
    cl_mem colors_buffer_rw;

    CL_Kernel create_rays_kernel;
    CL_Kernel trace_rays_kernel;
    CL_Kernel average_samples_kernel;
} CL_Instance;

bool load_cl_instance(CL_Instance *instance, const char *path);

bool prepare_kernels_cl(CL_Instance *instance, const Camera *camera,
                        const size_t width, const size_t height,
                        const Bvh *bvh);

bool raytrace_cl(const CL_Instance *instance, CL_Vec3 *colors);

void destroy_cl_instance(CL_Instance *instance);

#endif // CL_LOADER_H
