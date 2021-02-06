#include <math.h>
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

struct KCR_Matrix4 kcr_mat4_translate(float tx, float ty, float tz) {
    struct KCR_Matrix4 result = kcr_mat4_identity();
    result.m[0][3] = tx;
    result.m[1][3] = ty;
    result.m[2][3] = tz;

    return result;
}

struct KCR_Vec4 kcr_mat4_vec4_mult(const struct KCR_Matrix4* matrix, const struct KCR_Vec4* vector) {
    struct KCR_Vec4 result = {
            matrix->m[0][0] * vector->x + matrix->m[0][1] * vector->y + matrix->m[0][2] * vector->z + matrix->m[0][3] * vector->w,
            matrix->m[1][0] * vector->x + matrix->m[1][1] * vector->y + matrix->m[1][2] * vector->z + matrix->m[1][3] * vector->w,
            matrix->m[2][0] * vector->x + matrix->m[2][1] * vector->y + matrix->m[2][2] * vector->z + matrix->m[2][3] * vector->w,
            matrix->m[3][0] * vector->x + matrix->m[3][1] * vector->y + matrix->m[3][2] * vector->z + matrix->m[3][3] * vector->w,
    };

    return result;
}

struct KCR_Vec3 kcr_mat4_vec3_mult(const struct KCR_Matrix4* matrix, const struct KCR_Vec3* vector) {
    struct KCR_Vec4 vec4 = kcr_vec4_from_vec3(vector, 1);
    vec4 = kcr_mat4_vec4_mult(matrix, &vec4);
    struct KCR_Vec3 result = kcr_vec3_from_vec4(&vec4);

    return result;
}

struct KCR_Matrix4 kcr_mat4_mult(const struct KCR_Matrix4 *m1, const struct KCR_Matrix4 *m2) {
    struct KCR_Matrix4 result;
    result.m[0][0] = m1->m[0][0] * m2->m[0][0] + m1->m[0][1] * m2->m[1][0] + m1->m[0][2] * m2->m[2][0] + m1->m[0][3] * m2->m[3][0];
    result.m[0][1] = m1->m[0][0] * m2->m[0][1] + m1->m[0][1] * m2->m[1][1] + m1->m[0][2] * m2->m[2][1] + m1->m[0][3] * m2->m[3][1];
    result.m[0][2] = m1->m[0][0] * m2->m[0][2] + m1->m[0][1] * m2->m[1][2] + m1->m[0][2] * m2->m[2][2] + m1->m[0][3] * m2->m[3][2];
    result.m[0][3] = m1->m[0][0] * m2->m[0][3] + m1->m[0][1] * m2->m[1][3] + m1->m[0][2] * m2->m[2][3] + m1->m[0][3] * m2->m[3][3];
    result.m[1][0] = m1->m[1][0] * m2->m[0][0] + m1->m[1][1] * m2->m[1][0] + m1->m[1][2] * m2->m[2][0] + m1->m[1][3] * m2->m[3][0];
    result.m[1][1] = m1->m[1][0] * m2->m[0][1] + m1->m[1][1] * m2->m[1][1] + m1->m[1][2] * m2->m[2][1] + m1->m[1][3] * m2->m[3][1];
    result.m[1][2] = m1->m[1][0] * m2->m[0][2] + m1->m[1][1] * m2->m[1][2] + m1->m[1][2] * m2->m[2][2] + m1->m[1][3] * m2->m[3][2];
    result.m[1][3] = m1->m[1][0] * m2->m[0][3] + m1->m[1][1] * m2->m[1][3] + m1->m[1][2] * m2->m[2][3] + m1->m[1][3] * m2->m[3][3];
    result.m[2][0] = m1->m[2][0] * m2->m[0][0] + m1->m[2][1] * m2->m[1][0] + m1->m[2][2] * m2->m[2][0] + m1->m[2][3] * m2->m[3][0];
    result.m[2][1] = m1->m[2][0] * m2->m[0][1] + m1->m[2][1] * m2->m[1][1] + m1->m[2][2] * m2->m[2][1] + m1->m[2][3] * m2->m[3][1];
    result.m[2][2] = m1->m[2][0] * m2->m[0][2] + m1->m[2][1] * m2->m[1][2] + m1->m[2][2] * m2->m[2][2] + m1->m[2][3] * m2->m[3][2];
    result.m[2][3] = m1->m[2][0] * m2->m[0][3] + m1->m[2][1] * m2->m[1][3] + m1->m[2][2] * m2->m[2][3] + m1->m[2][3] * m2->m[3][3];
    result.m[3][0] = m1->m[3][0] * m2->m[0][0] + m1->m[3][1] * m2->m[1][0] + m1->m[3][2] * m2->m[2][0] + m1->m[3][3] * m2->m[3][0];
    result.m[3][1] = m1->m[3][0] * m2->m[0][1] + m1->m[3][1] * m2->m[1][1] + m1->m[3][2] * m2->m[2][1] + m1->m[3][3] * m2->m[3][1];
    result.m[3][2] = m1->m[3][0] * m2->m[0][2] + m1->m[3][1] * m2->m[1][2] + m1->m[3][2] * m2->m[2][2] + m1->m[3][3] * m2->m[3][2];
    result.m[3][3] = m1->m[3][0] * m2->m[0][3] + m1->m[3][1] * m2->m[1][3] + m1->m[3][2] * m2->m[2][3] + m1->m[3][3] * m2->m[3][3];

    return result;
}

struct KCR_Matrix4 kcr_mat4_rotation_x(float angleInRadians) {
    struct KCR_Matrix4 matrix = kcr_mat4_identity();
    matrix.m[1][1] = cosf(angleInRadians);
    matrix.m[1][2] = -sinf(angleInRadians);
    matrix.m[2][1] = sinf(angleInRadians);
    matrix.m[2][2] = cosf(angleInRadians);

    return matrix;
}

struct KCR_Matrix4 kcr_mat4_rotation_y(float angleInRadians) {
    struct KCR_Matrix4 matrix = kcr_mat4_identity();
    matrix.m[0][0] = cosf(angleInRadians);
    matrix.m[0][2] = sinf(angleInRadians);
    matrix.m[2][0] = -sinf(angleInRadians);
    matrix.m[2][2] = cosf(angleInRadians);

    return matrix;
}

struct KCR_Matrix4 kcr_mat4_rotation_z(float angleInRadians) {
    struct KCR_Matrix4 matrix = kcr_mat4_identity();
    matrix.m[0][0] = cosf(angleInRadians);
    matrix.m[0][1] = -sinf(angleInRadians);
    matrix.m[1][0] = sinf(angleInRadians);
    matrix.m[1][1] = cosf(angleInRadians);

    return matrix;
}