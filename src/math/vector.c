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
            scalar != 0 ? vector->x / scalar : 0,
            scalar != 0 ? vector->y / scalar : 0,
            scalar != 0 ? vector->z / scalar : 0,
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

struct KCR_Vec3 kcr_vec3_normalize(const struct KCR_Vec3 *vector) {
    struct KCR_Vec3 result = kcr_vec3_div(vector, kcr_vec3_length(vector));
    return result;
}
