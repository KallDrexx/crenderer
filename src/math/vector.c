#include <math.h>
#include "vector.h"

float kcr_vec3_length(const struct KCR_Vec3 *vector) {
    return sqrtf(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
}

struct KCR_Vec3 kcr_vec3_add(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second) {
    struct KCR_Vec3 result = {
        first->x + second->x,
        first->y + second->y,
        first->z + second->z,
    };

    return result;
}

struct KCR_Vec3 kcr_vec3_sub(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second) {
    struct KCR_Vec3 result = {
            first->x - second->x,
            first->y - second->y,
            first->z - second->z,
    };

    return result;
}

struct KCR_Vec3 kcr_vec3_mult(const struct KCR_Vec3 *vector, float scalar) {
    struct KCR_Vec3 result = {
            vector->x * scalar,
            vector->y * scalar,
            vector->z * scalar,
    };

    return result;
}

struct KCR_Vec3 kcr_vec3_div(const struct KCR_Vec3 *vector, float scalar) {
    struct KCR_Vec3 result = {
            vector->x / scalar,
            vector->y / scalar,
            vector->z / scalar,
    };

    return result;
}

float kcr_vec3_dot(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second) {
    return first->x * second->x +
        first->y * second->y +
        first->z * second->z;
}

struct KCR_Vec3 kcr_vec3_cross(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second) {
    struct KCR_Vec3 result = {
            first->y * second->z - first->z * second->y,
            first->z * second->x - first->x * second->z,
            first->x * second->y - first->y * second->x
    };

    return result;
}

struct KCR_Vec3 kcr_vec3_rotate_x(const struct KCR_Vec3 *vector, float angle) {
    struct KCR_Vec3 result = {
            vector->x,
            vector->y * cosf(angle) - vector->z * sinf(angle),
            vector->y * sinf(angle) + vector->z * cosf(angle),
    };

    return result;
}

struct KCR_Vec3 kcr_vec3_rotate_y(const struct KCR_Vec3 *vector, float angle) {
    struct KCR_Vec3 result = {
            vector->x * cosf(angle) - vector->z * sinf(angle),
            vector->y,
            vector->x * sinf(angle) + vector->z * cosf(angle),
    };

    return result;
}

struct KCR_Vec3 kcr_vec3_rotate_z(const struct KCR_Vec3 *vector, float angle) {
    struct KCR_Vec3 result = {
            vector->x * cosf(angle) - vector->y * sinf(angle),
            vector->x * sinf(angle) + vector->y * cosf(angle),
            vector->z,
    };

    return result;
}
