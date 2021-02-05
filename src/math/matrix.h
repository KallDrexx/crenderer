#ifndef CRENDERER_MATRIX_H
#define CRENDERER_MATRIX_H

#include "vector.h"

struct KCR_Matrix4 {
    float m[4][4];
};

struct KCR_Matrix4 kcr_mat4_identity();
struct KCR_Matrix4 kcr_mat4_scale(float scaleX, float scaleY, float scaleZ);
struct KCR_Matrix4 kcr_mat4_mult(const struct KCR_Matrix4* m1, const struct KCR_Matrix4* m2);
struct KCR_Vec3 kcr_mat4_vec3_mult(const struct KCR_Matrix4* matrix, const struct KCR_Vec3* vector);

#endif //CRENDERER_MATRIX_H
