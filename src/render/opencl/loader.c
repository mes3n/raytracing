#include "loader.h"

#include <CL/cl.h>

#include <assert.h>
#include <stdlib.h>

#include <stdio.h>

static const char *program_source = "./kernels/main.cl";
static const char *create_rays_name = "create_rays";
static const char *trace_rays_name = "trace_rays";

static cl_kernel create_rays_kernel;
static cl_kernel trace_rays_kernel;
static cl_context context;
static cl_command_queue queue;

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

    char *build_log = malloc(log_size);
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                          sizeof(build_log), build_log, NULL);

    printf("Kernel Build Error:\n%s\n", build_log);
    free(build_log);
}

bool load_cl_kernel() {
    cl_int err = CL_SUCCESS;

    cl_platform_id platform_id;
    err = clGetPlatformIDs(1, &platform_id, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to get platform IDs.\n");
        return false;
    }

    cl_device_id device_id;
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
                         NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to get device IDs.\n");
        return false;
    }

    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create device context.\n");
        return false;
    }
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create command queue.\n");
        return false;
    }

    const char *program_string = load_file(program_source);
    if (program_string == NULL) {
        fprintf(stderr, "CL_Loader: Failed to read program source: %s\n",
                program_source);
        goto release_context;
    }

    cl_program program =
        clCreateProgramWithSource(context, 1, &program_string, NULL, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create program from source.\n");
        print_program_build_error(program, device_id);
        goto free_program_string;
    }

    const char *options = "-I kernels -cl-single-precision-constant "
                          "-cl-mad-enable -cl-fast-relaxed-math";
    err = clBuildProgram(program, 1, &device_id, options, NULL, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to build program. %d.\n", err);
        print_program_build_error(program, device_id);
        goto release_program;
    }

    create_rays_kernel = clCreateKernel(program, create_rays_name, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create kernel: %s.\n",
                create_rays_name);
        print_program_build_error(program, device_id);
        goto release_program;
    }

    trace_rays_kernel = clCreateKernel(program, trace_rays_name, &err);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "CL_Loader: Failed to create kernel: %s.\n",
                trace_rays_name);
        print_program_build_error(program, device_id);
        goto release_program;
    }

release_program:
    // clReleaseProgram(program);
free_program_string:
    free((void *)program_string);
release_context:
    // clReleaseContext(context);

    return err == CL_SUCCESS;
}

CL_Ray *create_rays_cl(CL_Camera *camera, const int width, const int height,
                       size_t *rays_size) {
    cl_int err;

    cl_mem camera_buffer =
        clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       sizeof(CL_Camera), camera, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating camera buffer: %d\n", err);
        return NULL;
    }
    cl_uint seed = rand();
    cl_mem seed_buffer =
        clCreateBuffer(context, CL_MEM_READ_WRITE,
                       sizeof(cl_uint), &seed, &err);
    *rays_size = width * height * camera->samples_per_pixel;
    cl_mem ray_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                       *rays_size * sizeof(CL_Ray), NULL, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating ray buffer: %d\n", err);
        return NULL;
    }

    clSetKernelArg(create_rays_kernel, 0, sizeof(cl_mem), &camera_buffer);
    clSetKernelArg(create_rays_kernel, 1, sizeof(cl_mem), &seed_buffer);
    clSetKernelArg(create_rays_kernel, 2, sizeof(cl_mem), &ray_buffer);

    size_t global_work_size[] = {width, height};
    err = clEnqueueNDRangeKernel(queue, create_rays_kernel, 2, NULL,
                                 global_work_size, NULL, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating create_rays kernel task: %d\n", err);
        return NULL;
    }

    CL_Ray *rays = (CL_Ray *)malloc(*rays_size * sizeof(CL_Ray));
    clEnqueueReadBuffer(queue, ray_buffer, CL_TRUE, 0,
                        *rays_size * sizeof(CL_Ray), rays, 0, NULL, NULL);

    return rays;
}

CL_Vec3 *trace_rays_cl(CL_Ray *rays, const size_t rays_size, CL_Bvh *bvhs,
                       const size_t bvhs_size, CL_Hittable *shapes,
                       const size_t shapes_size, CL_Material *materials,
                       const size_t materials_size, const unsigned int depth) {
    assert(bvhs_size > 0);

    cl_int err;
    cl_mem rays_buffer =
        clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       rays_size * sizeof(CL_Ray), rays, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating rays buffer: %d\n", err);
        return NULL;
    }
    cl_mem bvhs_buffer =
        clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       bvhs_size * sizeof(CL_Bvh), bvhs, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating bvhs buffer: %d\n", err);
        return NULL;
    }
    cl_mem shapes_buffer =
        clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       shapes_size * sizeof(CL_Hittable), shapes, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating shapes buffer: %d\n", err);
        return NULL;
    }
    cl_mem materials_buffer =
        clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       materials_size * sizeof(CL_Material), materials, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating materials buffer: %d\n", err);
        return NULL;
    }
    cl_mem colors_buffer = clCreateBuffer(
        context, CL_MEM_WRITE_ONLY, rays_size * sizeof(CL_Vec3), NULL, &err);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating colors buffer: %d\n", err);
        return NULL;
    }

    clSetKernelArg(trace_rays_kernel, 0, sizeof(cl_mem), &rays_buffer);
    clSetKernelArg(trace_rays_kernel, 1, sizeof(cl_mem), &bvhs_buffer);
    clSetKernelArg(trace_rays_kernel, 2, sizeof(cl_mem), &shapes_buffer);
    clSetKernelArg(trace_rays_kernel, 3, sizeof(cl_mem), &materials_buffer);
    clSetKernelArg(trace_rays_kernel, 4, sizeof(unsigned int), &depth);
    clSetKernelArg(trace_rays_kernel, 5, sizeof(cl_mem), &colors_buffer);

    err = clEnqueueNDRangeKernel(queue, trace_rays_kernel, 1, NULL, &rays_size,
                                 NULL, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("CL_Loader: Error creating kernel task: %d\n", err);
        return NULL;
    }

    CL_Vec3 *colors = (CL_Vec3 *)malloc(rays_size * sizeof(CL_Vec3));
    clEnqueueReadBuffer(queue, colors_buffer, CL_TRUE, 0,
                        rays_size * sizeof(CL_Vec3), colors, 0, NULL, NULL);

    return colors;
}
