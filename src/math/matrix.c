#include "matrix.h"

struct KCR_Matrix4 kcr_mat4_identity() {
    struct KCR_Matrix4 result = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }};
    return result;
}

struct KCR_Matrix4 kcr_mat4_scale(float scaleX, float scaleY, float scaleZ) {
    struct KCR_Matrix4 result = kcr_mat4_identity();
    result.m[0][0] = scaleX;
    result.m[1][1] = scaleY;
    result.m[2][2] = scaleZ;

    return result;
}

struct KCR_Vec3 kcr_mat4_vec3_mult(const struct KCR_Matrix4 *matrix, const struct KCR_Vec3 *vector) {
    struct KCR_Vec3 result = {
            matrix->m[0][0] * vector->x + matrix->m[0][1] * vector->y + matrix->m[0][2] * vector->z,
            matrix->m[1][0] * vector->x + matrix->m[1][1] * vector->y + matrix->m[1][2] * vector->z,
            matrix->m[2][0] * vector->x + matrix->m[2][1] * vector->y + matrix->m[2][2] * vector->z,
    };

    return result;
}