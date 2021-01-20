#include <math.h>
#include "vector.h"

float vec3_length(struct KCR_Vec3 *vector) {
    return sqrtf(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
}

struct KCR_Vec3 vec3_add(struct KCR_Vec3 *first, struct KCR_Vec3 *second) {
    struct KCR_Vec3 result = {
        first->x + second->x,
        first->y + second->y,
        first->z + second->z,
    };

    return result;
}

struct KCR_Vec3 vec3_sub(struct KCR_Vec3 *first, struct KCR_Vec3 *second) {
    struct KCR_Vec3 result = {
            first->x - second->x,
            first->y - second->y,
            first->z - second->z,
    };

    return result;
}

struct KCR_Vec3 vec3_mult(struct KCR_Vec3 *vector, float scalar) {
    struct KCR_Vec3 result = {
            vector->x * scalar,
            vector->y * scalar,
            vector->z * scalar,
    };

    return result;
}

struct KCR_Vec3 vec3_div(struct KCR_Vec3 *vector, float scalar) {
    struct KCR_Vec3 result = {
            vector->x / scalar,
            vector->y / scalar,
            vector->z / scalar,
    };

    return result;
}

float vec3_dot(struct KCR_Vec3 *first, struct KCR_Vec3 *second) {
    return first->x * second->x +
        first->y * second->y +
        first->z * second->z;
}

struct KCR_Vec3 vec3_cross(struct KCR_Vec3 *first, struct KCR_Vec3 *second) {
    struct KCR_Vec3 result = {
            first->y * second->z - first->z * second->y,
            first->z * second->x - first->x * second->z,
            first->x * second->y - first->y * second->x
    };

    return result;
}

struct KCR_Vec3 vec3_rotate_x(struct KCR_Vec3 *vector, float angle) {
    struct KCR_Vec3 result = {
            vector->x,
            vector->y * cosf(angle) - vector->z * sinf(angle),
            vector->y * sinf(angle) + vector->z * cosf(angle),
    };

    return result;
}

struct KCR_Vec3 vec3_rotate_y(struct KCR_Vec3 *vector, float angle) {
    struct KCR_Vec3 result = {
            vector->x * cosf(angle) - vector->z * sinf(angle),
            vector->y,
            vector->x * sinf(angle) + vector->z * cosf(angle),
    };

    return result;
}

struct KCR_Vec3 vec3_rotate_z(struct KCR_Vec3 *vector, float angle) {
    struct KCR_Vec3 result = {
            vector->x * cosf(angle) - vector->y * sinf(angle),
            vector->x * sinf(angle) + vector->y * cosf(angle),
            vector->z,
    };

    return result;
}
