#ifndef COMMON_H
#define COMMON_H

#ifndef RAY_CL_AS_H
#ifndef SIZE_MAX
#define SIZE_MAX 0xffffffff
#endif // SIZE_MAX
typedef float3 CL_Vec3;
typedef float CL_float;
typedef int CL_int;
typedef bool CL_bool;
#define RAY_CL_PREFIX(name) name
#else
#include <stddef.h>
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#define RAY_CL_PREFIX(name) __attribute__((aligned(16))) CL_##name
typedef cl_float3 CL_Vec3;
typedef cl_float CL_float;
typedef cl_int CL_int;
typedef cl_bool CL_bool;
#endif // RAY_CL_AS_H

/**
 * Struct to represent a ray in space using its origin and direction.
 */
typedef struct {
    CL_Vec3 origin;
    CL_Vec3 direction;
    CL_float time;
} RAY_CL_PREFIX(Ray);

/**
 * Internal struct to represent to camera's viewport
 */
typedef struct {
    CL_Vec3 u, v;
    CL_Vec3 pos_at00;
} RAY_CL_PREFIX(Viewport);

/**
 * Struct to represent a `Camera` object
 *
 * The struct must be initialized using the `init_camera` function
 * before it can be used. All fields will be set automatically and should
 * not be changed manually.
 */
typedef struct {
    CL_int samples_per_pixel;
    CL_int max_depth;

    CL_Vec3 origin;

    CL_float defocus_angle;
    CL_Vec3 defocus_disc_u;
    CL_Vec3 defocus_disc_v;

    RAY_CL_PREFIX(Viewport) viewport;
} RAY_CL_PREFIX(Camera);

typedef struct {
    CL_float min;
    CL_float max;
} RAY_CL_PREFIX(Interval);

/**
 * Struct to represent an axis aligned bounding box.
 *
 * @param x,y,z Intervals of each axis the `Bbox` covers
 */
typedef struct {
    CL_Vec3 min, max;
} RAY_CL_PREFIX(Bbox);

typedef struct {
    enum { LAMBERTIAN, METAL, DIELECTRIC } mat_type;
    union {
        struct Lambertian {
            CL_Vec3 albedo;
        } lambertian;
        struct Metal {
            CL_Vec3 albedo;
            CL_float fuzz;
        } metal;
        struct Dielectric {
            CL_float refraction;
        } dielectric;
    } mat;
} RAY_CL_PREFIX(Material);

/**
 * All shape structs must implement these fields first and in correct order.
 * This can be done with the `DERIVE_HITTABLE()` macro.
 *
 * @param material Pointer to material shape is made of
 * @param hit Function pointer to calculate if shape has been hit
 * @pararm bbox The bounding box of the `Hittable`
 */
typedef struct {
    enum { SPHERE, QUAD } shape_type;
    union {
        struct Quad {
            CL_Vec3 origin;
            CL_Vec3 u, v;

            CL_Vec3 normal;
            CL_Vec3 w;
            CL_float d;
        } quad;
        struct Sphere {
            CL_Vec3 center;
            CL_Vec3 dcenter;
            CL_float radius;
        } sphere;
    } shape;

    size_t material_idx;
    RAY_CL_PREFIX(Bbox) bbox;
} RAY_CL_PREFIX(Hittable);

/**
 * Struct representing a bounding volume hierarchy node.
 *
 * @param bbox The `Bvh` node's `Bbox`
 * @param left Its left child, NULL if leaf
 * @param right Its right child, NULL if leaf
 * @param shape A pointer to its shape, NULL unless leaf
 */
typedef struct {
    RAY_CL_PREFIX(Bbox) bbox;
    size_t left_idx;
    size_t right_idx;
    size_t hittable_idx;
} RAY_CL_PREFIX(Bvh);

/**
 * Public struct to represent the values generated when a ray hits an object
 * in the world.
 *
 * @param point A point in space where the ray intersected an object
 * @param normal Normal vector of the object where it was hit by the ray
 * @param t Distance traveled by the ray
 * @param front_face Was the hit on the back or front of object
 */
typedef struct {
    CL_Vec3 point;
    CL_Vec3 normal;
    CL_float t;
    CL_bool front_face;

    size_t material_idx;

    CL_float u, v;
} RAY_CL_PREFIX(HitRecord);

#endif // COMMON_H
