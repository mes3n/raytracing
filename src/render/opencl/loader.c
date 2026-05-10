#include "loader.h"
#include "src/render/opencl/conversion.h"

#include <CL/cl.h>

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include <stdio.h>
#include <time.h>

#define create_rays_name "create_rays"
#define trace_rays_name "trace_rays"
#define averagle_samples_name "average_samples"

inline const char *load_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Error: Could not open %s\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *str = (char *)malloc(size + 1);
    fread(str, 1, size, f);
    fclose(f);

    str[size] = '\0';
    return str;
}

void print_program_build_error(cl_program program, cl_device_id device_id) {
    size_t log_size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL,
                          &log_size);

    char *build_log = (char *)malloc(log_size);
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                          sizeof(build_log), build_log, NULL);

    printf("Kernel Build Error:\n%s\n", build_log);
    free(build_log);
}

bool load_cl_instance(CL_Instance *instance, const char *path) {
    cl_int err = CL_SUCCESS;

    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to get platform IDs.\n");
        return false;
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &instance->device,
                         NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to get device IDs. %d.\n", err);
        return false;
    }

    instance->context =
        clCreateContext(NULL, 1, &instance->device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create device context.\n");
        return false;
    }
    instance->queue = clCreateCommandQueueWithProperties(
        instance->context, instance->device, 0, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create command queue.\n");
        return false;
    }

    const char *program_string = load_file(path);
    if (program_string == NULL) {
        fprintf(stderr, "CL_Loader: Failed to read program at %s\n", path);
        return false;
    }

    cl_program program = clCreateProgramWithSource(instance->context, 1,
                                                   &program_string, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create program from source.\n");
        print_program_build_error(program, instance->device);
        goto free_program_string;
    }

    const char *options = "-I ./kernels -cl-single-precision-constant "
                          "-cl-mad-enable -cl-fast-relaxed-math";
    err = clBuildProgram(program, 1, &instance->device, options, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to build program. %d.\n", err);
        print_program_build_error(program, instance->device);
        goto release_program;
    }

    instance->create_rays_kernel.id =
        clCreateKernel(program, create_rays_name, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create kernel: %s.\n",
                create_rays_name);
        print_program_build_error(program, instance->device);
        goto release_program;
    }

    instance->trace_rays_kernel.id =
        clCreateKernel(program, trace_rays_name, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create kernel: %s.\n",
                trace_rays_name);
        print_program_build_error(program, instance->device);
        goto release_program;
    }

    instance->average_samples_kernel.id =
        clCreateKernel(program, averagle_samples_name, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create kernel: %s.\n",
                averagle_samples_name);
        print_program_build_error(program, instance->device);
        goto release_program;
    }

release_program:
    clReleaseProgram(program);
free_program_string:
    free((void *)program_string);

    return err == CL_SUCCESS;
}

bool prepare_kernels_cl(CL_Instance *instance, const Camera *camera,
                        const size_t width, const size_t height,
                        const Bvh *bvh) {
    CL_Camera cl_camera = convert_camera(camera);

    const size_t bvh_count = bvh_count_nodes(bvh);
    const size_t ht_count = bvh_count_leaves(bvh);

    CL_Bvh *cl_bvhs = malloc(bvh_count * sizeof(CL_Bvh));
    CL_Hittable *cl_shapes = malloc(ht_count * sizeof(CL_Hittable));
    CL_Material *cl_materials = malloc(ht_count * sizeof(CL_Material));
    convert_bvh(bvh, cl_bvhs, cl_shapes, cl_materials);

    cl_int err;
    const size_t colors_size = width * height;
    const size_t rays_size = colors_size * camera->samples_per_pixel;
    const cl_uint seed = time(NULL);

    instance->create_rays_kernel.work_dim = 2;
    instance->create_rays_kernel.work_size[0] = width;
    instance->create_rays_kernel.work_size[1] = height;

    instance->trace_rays_kernel.work_dim = 1;
    instance->trace_rays_kernel.work_size[0] = rays_size;

    instance->average_samples_kernel.work_dim = 1;
    instance->average_samples_kernel.work_size[0] = colors_size;

    const cl_mem_flags ro_flags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;

    instance->camera_buffer = clCreateBuffer(
        instance->context, ro_flags, sizeof(CL_Camera), &cl_camera, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating camera buffer: %d\n", err);
        return false;
    }

    size_t max_alloc_size;
    clGetDeviceInfo(instance->device, CL_DEVICE_MAX_MEM_ALLOC_SIZE,
                    sizeof(max_alloc_size), &max_alloc_size, NULL);
    assert(rays_size * sizeof(CL_Ray) < max_alloc_size);

    instance->rays_buffer_rw =
        clCreateBuffer(instance->context, CL_MEM_READ_WRITE,
                       rays_size * sizeof(CL_Ray), NULL, &err);
    if (err != CL_SUCCESS) {

        printf("CL_Loader: Error creating rays buffer: %d\n", err);
        return false;
    }
    instance->bvhs_buffer = clCreateBuffer(
        instance->context, ro_flags, bvh_count * sizeof(CL_Bvh), cl_bvhs, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating bvhs buffer: %d\n", err);
        return false;
    }
    instance->shapes_buffer =
        clCreateBuffer(instance->context, ro_flags,
                       ht_count * sizeof(CL_Hittable), cl_shapes, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating shapes buffer: %d\n", err);
        return false;
    }
    instance->materials_buffer =
        clCreateBuffer(instance->context, ro_flags,
                       ht_count * sizeof(CL_Material), cl_materials, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating materials buffer: %d\n", err);
        return false;
    }
    instance->colors_buffer_rw =
        clCreateBuffer(instance->context, CL_MEM_READ_WRITE,
                       rays_size * sizeof(CL_Vec3), NULL, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating colors buffer: %d\n", err);
        return false;
    }

    free(cl_bvhs);
    free(cl_shapes);
    free(cl_materials);

    clSetKernelArg(instance->create_rays_kernel.id, 0, sizeof(cl_mem),
                   &instance->camera_buffer);
    clSetKernelArg(instance->create_rays_kernel.id, 1, sizeof(cl_uint), &seed);
    clSetKernelArg(instance->create_rays_kernel.id, 2, sizeof(cl_mem),
                   &instance->rays_buffer_rw);

    clSetKernelArg(instance->trace_rays_kernel.id, 0, sizeof(cl_mem),
                   &instance->rays_buffer_rw);
    clSetKernelArg(instance->trace_rays_kernel.id, 1, sizeof(cl_mem),
                   &instance->bvhs_buffer);
    clSetKernelArg(instance->trace_rays_kernel.id, 2, sizeof(cl_mem),
                   &instance->shapes_buffer);
    clSetKernelArg(instance->trace_rays_kernel.id, 3, sizeof(cl_mem),
                   &instance->materials_buffer);
    clSetKernelArg(instance->trace_rays_kernel.id, 4, sizeof(cl_uint),
                   &camera->max_depth);
    clSetKernelArg(instance->trace_rays_kernel.id, 5, sizeof(cl_uint), &seed);
    clSetKernelArg(instance->trace_rays_kernel.id, 6, sizeof(cl_mem),
                   &instance->colors_buffer_rw);

    clSetKernelArg(instance->average_samples_kernel.id, 0, sizeof(cl_mem),
                   &instance->colors_buffer_rw);
    clSetKernelArg(instance->average_samples_kernel.id, 1, sizeof(cl_uint),
                   &camera->samples_per_pixel);

    return true;
}

bool raytrace_cl(const CL_Instance *instance, CL_Vec3 *colors) {
    cl_int err;

    cl_event event_queue[14] = {0};
    cl_uint qi = 0;

    err =
        clEnqueueNDRangeKernel(instance->queue, instance->create_rays_kernel.id,
                               instance->create_rays_kernel.work_dim, NULL,
                               instance->create_rays_kernel.work_size, NULL, qi,
                               NULL, event_queue + qi);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating create_rays kernel task: %d\n", err);
        return false;
    }
    qi++;

    size_t rays_size = instance->trace_rays_kernel.work_size[0];
    err = clEnqueueNDRangeKernel(
        instance->queue, instance->trace_rays_kernel.id, 1, NULL, &rays_size,
        NULL, qi, event_queue, event_queue + qi);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating trace_rays kernel task: %d\n", err);
        return false;
    }
    qi++;

    err = clEnqueueNDRangeKernel(
        instance->queue, instance->average_samples_kernel.id,
        instance->average_samples_kernel.work_dim, NULL,
        instance->average_samples_kernel.work_size, NULL, qi, event_queue,
        event_queue + qi);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating average_samples kernel task: %d\n",
               err);
        return false;
    }
    qi++;

    err = clEnqueueReadBuffer(instance->queue, instance->colors_buffer_rw,
                              CL_TRUE, 0, rays_size * sizeof(CL_Vec3), colors,
                              qi, event_queue, event_queue + qi);
    qi++;

    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error reading colors_buffer: %d\n", err);
        return false;
    }

    return true;
}

void destroy_cl_instance(CL_Instance *instance) {
    clReleaseMemObject(instance->bvhs_buffer);
    clReleaseMemObject(instance->shapes_buffer);
    clReleaseMemObject(instance->materials_buffer);
    clReleaseMemObject(instance->colors_buffer_rw);

    clReleaseMemObject(instance->camera_buffer);
    clReleaseMemObject(instance->rays_buffer_rw);

    clReleaseKernel(instance->create_rays_kernel.id);
    clReleaseKernel(instance->trace_rays_kernel.id);
    clReleaseKernel(instance->average_samples_kernel.id);

    clReleaseDevice(instance->device);
    clReleaseContext(instance->context);
    clReleaseCommandQueue(instance->queue);
}
