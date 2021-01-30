#ifndef CRENDERER_VECTOR_H
#define CRENDERER_VECTOR_H

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
 * Rotates a KCR_Vec3 by the specified angle (in radians) on the x axis, producing a new KCR_Vec3 with the results
 */
struct KCR_Vec3 kcr_vec3_rotate_x(const struct KCR_Vec3 *vector, float angle);

/*
 * Rotates a KCR_Vec3 by the specified angle (in radians) on the y axis, producing a new KCR_Vec3 with the results
 */
struct KCR_Vec3 kcr_vec3_rotate_y(const struct KCR_Vec3 *vector, float angle);

/*
 * Rotates a KCR_Vec3 by the specified angle (in radians) on the z axis, producing a new KCR_Vec3 with the results
 */
struct KCR_Vec3 kcr_vec3_rotate_z(const struct KCR_Vec3 *vector, float angle);

/*
 * Normalizes a KCR_Vec3 to create a unit vector, producing a new KCR_Vec3 with the result
 */
struct KCR_Vec3 kcr_vec3_normalize(const struct KCR_Vec3 *vector);

#endif //CRENDERER_VECTOR_H
