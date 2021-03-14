#ifndef CRENDERER_VECTOR_H
#define CRENDERER_VECTOR_H

#include <math.h>

/*
 * A vector with 2 components
 */
struct KCR_Vec2 {
    float x;
    float y;
};

/*
 * A vector with 3 components
 */
struct KCR_Vec3 {
    float x;
    float y;
    float z;
};

/*
 * A vector with 4 components
 */
struct KCR_Vec4 {
    float x;
    float y;
    float z;
    float w;
};

/*
 * Gets the magnitude of a KCR_Vec3
 */
float kcr_vec3_length(const struct KCR_Vec3 *vector);

/*
 * Adds two KCR_Vec3s together, producing a new KCR_Vec3 with the result
 */
struct KCR_Vec3 kcr_vec3_add(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);

/*
 * Subtracts two KCR_Vec3s from each other, producing a new KCR_Vec3 with the result
 */
struct KCR_Vec3 kcr_vec3_sub(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);

/*
 * Multiplies a KCR_Vec3 by a scalar, producing a new KCR_Vec3 with the result
 */
struct KCR_Vec3 kcr_vec3_mult(const struct KCR_Vec3 *vector, float scalar);

/*
 * Divides a KCR_Vec3 by a scalar, producing a new KCR_Vec3 with the result
 */
struct KCR_Vec3 kcr_vec3_div(const struct KCR_Vec3 *vector, float scalar);

/*
 * Performs the dot product operation against two KCR_Vec3s
 */
float kcr_vec3_dot(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);

/*
 * Performs the cross product operation against two KCR_Vec3s
 */
struct KCR_Vec3 kcr_vec3_cross(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);

/*
 * Normalizes a KCR_Vec3 to create a unit vector, producing a new KCR_Vec3 with the result
 */
struct KCR_Vec3 kcr_vec3_normalize(const struct KCR_Vec3* vector);

/*
 * Rotates a 3 component vector around the z axis
 */
static inline struct KCR_Vec3 kcr_vec3_rotate_z(const struct KCR_Vec3* vector, float angle) {
    return (struct KCR_Vec3) {
            .x = vector->x,
            .y = vector->y * cosf(angle) - vector->z * sinf(angle),
            .z = vector->y * sinf(angle) + vector->z * cosf(angle),
    };
}

/*
 * Rotates a 3 component vector around the y axis
 */
static inline struct KCR_Vec3 kcr_vec3_rotate_y(const struct KCR_Vec3* vector, float angle) {
    return (struct KCR_Vec3) {
        .x = vector->z * sinf(angle) + vector->x * cosf(angle),
        .y = vector->y,
        .z = vector->z * cosf(angle) - vector->x * sinf(angle),
    };
}

/*
 * Rotates a 3 component vector around the x axis
 */
static inline struct KCR_Vec3 kcr_vec3_rotate_x(const struct KCR_Vec3* vector, float angle) {
    return (struct KCR_Vec3) {
            .x = vector->x,
            .y = vector->y * cosf(angle) - vector->z * sinf(angle),
            .z = vector->y * sinf(angle) + vector->z * cosf(angle),
    };
}

/*
 * Subtracts two 4 component vectors
 */
static inline struct KCR_Vec4 kcr_vec4_sub(const struct KCR_Vec4* first, const struct KCR_Vec4* second) {
    return (struct KCR_Vec4) {
        .x = first->x - second->x,
        .y = first->y - second->y,
        .z = first->z - second->z,
        .w = first->w - second->w,
    };
}

/*
 * Divides a 4 component vector by a scalar
 */
static inline struct KCR_Vec4 kcr_vec4_div(const struct KCR_Vec4* vector, float scalar) {
    return (struct KCR_Vec4) {
        .x = vector->x / scalar,
        .y = vector->y / scalar,
        .z = vector->z / scalar,
        .w = vector->w / scalar,
    };
}

/*
 * Creates a 4 component vector from a 3 component vector with the specified w component
 */
static inline struct KCR_Vec4 kcr_vec4_from_vec3(const struct KCR_Vec3* vec3, float w) {
    struct KCR_Vec4 result = {vec3->x, vec3->y, vec3->z, w};
    return result;
}

/*
 * Creates a 3 component vector from a 4 component vector, ignoring the w component
 */
static inline struct KCR_Vec3 kcr_vec3_from_vec4(const struct KCR_Vec4* vec4) {
    struct KCR_Vec3 result = {vec4->x, vec4->y, vec4->z};
    return result;
}

/*
 * Creates a 2 component vector from a 4 component vector, ignoring the z and w components.
 */
static inline struct KCR_Vec2 kcr_vec2_from_vec4(const struct KCR_Vec4* vec4) {
    struct KCR_Vec2 result = {vec4->x, vec4->y};

    return result;
}

/*
 * Subtracts a 2 component vector from another
 */
static inline struct KCR_Vec2 kcr_vec2_sub(const struct KCR_Vec2* first, const struct KCR_Vec2* second) {
    return (struct KCR_Vec2) {
        .x = first->x - second->x,
        .y = first->y - second->y,
    };
}

/*
 * Adds a 2 component vector from another
 */
static inline struct KCR_Vec2 kcr_vec2_add(const struct KCR_Vec2* first, const struct KCR_Vec2* second) {
    return (struct KCR_Vec2) {
            .x = first->x + second->x,
            .y = first->y + second->y,
    };
}

/*
 * Divides a 2 component vector by a scalar
 */
static inline struct KCR_Vec2 kcr_vec2_div(const struct KCR_Vec2* vec, float scalar) {
    return (struct KCR_Vec2) {
        .x = vec->x / scalar,
        .y = vec->y / scalar,
    };
}

/*
 * Multiplies a 2 component vector by a scalar
 */
static inline struct KCR_Vec2 kcr_vec2_mult(const struct KCR_Vec2* vec, float scalar) {
    return (struct KCR_Vec2) {
            .x = vec->x * scalar,
            .y = vec->y * scalar,
    };
}

#endif //CRENDERER_VECTOR_H
