#ifndef CRENDERER_VECTOR_H
#define CRENDERER_VECTOR_H

struct KCR_Vec2 {
    float x;
    float y;
};

struct KCR_Vec3 {
    float x;
    float y;
    float z;
};

float kcr_vec3_length(const struct KCR_Vec3 *vector);
struct KCR_Vec3 kcr_vec3_add(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);
struct KCR_Vec3 kcr_vec3_sub(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);
struct KCR_Vec3 kcr_vec3_mult(const struct KCR_Vec3 *vector, float scalar);
struct KCR_Vec3 kcr_vec3_div(const struct KCR_Vec3 *vector, float scalar);
float kcr_vec3_dot(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);
struct KCR_Vec3 kcr_vec3_cross(const struct KCR_Vec3 *first, const struct KCR_Vec3 *second);
struct KCR_Vec3 kcr_vec3_rotate_x(const struct KCR_Vec3 *vector, float angle);
struct KCR_Vec3 kcr_vec3_rotate_y(const struct KCR_Vec3 *vector, float angle);
struct KCR_Vec3 kcr_vec3_rotate_z(const struct KCR_Vec3 *vector, float angle);

#endif //CRENDERER_VECTOR_H
