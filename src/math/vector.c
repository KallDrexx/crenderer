#include <math.h>
#include "vector.h"

float vec3_length(Vec3 *vector) {
    return sqrtf(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
}

Vec3 vec3_add(Vec3 *first, Vec3 *second) {
    Vec3 result = {
        first->x + second->x,
        first->y + second->y,
        first->z + second->z,
    };

    return result;
}

Vec3 vec3_sub(Vec3 *first, Vec3 *second) {
    Vec3 result = {
            first->x - second->x,
            first->y - second->y,
            first->z - second->z,
    };

    return result;
}

Vec3 vec3_mult(Vec3 *vector, float scalar) {
    Vec3 result = {
            vector->x * scalar,
            vector->y * scalar,
            vector->z * scalar,
    };

    return result;
}

Vec3 vec3_div(Vec3 *vector, float scalar) {
    Vec3 result = {
            vector->x / scalar,
            vector->y / scalar,
            vector->z / scalar,
    };

    return result;
}

float vec3_dot(Vec3 *first, Vec3 *second) {
    return first->x * second->x +
        first->y * second->y +
        first->z * second->z;
}

Vec3 vec3_cross(Vec3 *first, Vec3 *second) {
    Vec3 result = {
            first->y * second->z - first->z * second->y,
            first->z * second->x - first->x * second->z,
            first->x * second->y - first->y * second->x
    };

    return result;
}


