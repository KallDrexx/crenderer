#ifndef CRENDERER_MATRIX_H
#define CRENDERER_MATRIX_H

#include "vector.h"

/*
 * Represents a 4x4 matrix
 */
struct KCR_Matrix4 {
    float m[4][4];
};

/*
 * Returns a 4x4 identity matrix
 */
struct KCR_Matrix4 kcr_mat4_identity();

/*
 * Creates a 4x4 matrix representing a scale transformation
 */
struct KCR_Matrix4 kcr_mat4_scale(float scaleX, float scaleY, float scaleZ);

/*
 * Creates a 4x4 matrix representing a translation transformation
 */
struct KCR_Matrix4 kcr_mat4_translate(float tx, float ty, float tz);

/*
 * Creates a 4x4 matrix representing a rotation pm the X axis by the specified angle
 */
struct KCR_Matrix4 kcr_mat4_rotation_x(float angleInRadians);

/*
 * Creates a 4x4 matrix representing a rotation on the Y axis by the specified angle
 */
struct KCR_Matrix4 kcr_mat4_rotation_y(float angleInRadians);

/*
 * Creates a 4x4 matrix representing a rotation on the Z axis by the specified angle
 */
struct KCR_Matrix4 kcr_mat4_rotation_z(float angleInRadians);

/*
 * Creates a 4x4 matrix representing a perspective projection matrix
 */
struct KCR_Matrix4 kcr_mat4_perspective(float horizontalFov, float aspectRatio, float zNear, float zFar);

/*
 * Multiplies two 4x4 matrices together
 */
struct KCR_Matrix4 kcr_mat4_mult(const struct KCR_Matrix4* m1, const struct KCR_Matrix4* m2);

/*
 * Multiplies a 4x4 matrix against a 3 component vector.  The vector will be auto-scaled to a 4 component vector
 * for this to work.
 */
struct KCR_Vec3 kcr_mat4_vec3_mult(const struct KCR_Matrix4* matrix, const struct KCR_Vec3* vector);

/*
 * Multiplies a 4x4 matrix against a 4 component vector.
 */
struct KCR_Vec4 kcr_mat4_vec4_mult(const struct KCR_Matrix4* matrix, const struct KCR_Vec4* vector);

#endif //CRENDERER_MATRIX_H
