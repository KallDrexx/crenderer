#include <math.h>
#include "vector.h"

float vector3_length(Vector3* vector) {
    float total = vector->x * vector->x +
            vector->y * vector->y +
            vector->z * vector->z;

    return sqrtf(total);
}

float vector2_length(Vector2* vector) {
    float total = vector->x * vector->x +
            vector->y * vector->y;

    return sqrtf(total);
}

Vector3 vector3_add(Vector3 *first, Vector3 *second) {
    Vector3 result = {
            first->x + second->x,
            first->y + second->y,
            first->z + second->z,
    };

    return result;
}

Vector2 vector2_add(Vector2 *first, Vector2 *second) {
    Vector2 result = {
            first->x + second->x,
            first->y + second->y,
    };

    return result;
}

Vector2 project_to_2d(Vector3 *vector) {
    Vector2 result = { vector->x, vector->y };
    return result;
}
