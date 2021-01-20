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

float vec3_length(struct KCR_Vec3 *vector);
struct KCR_Vec3 vec3_add(struct KCR_Vec3 *first, struct KCR_Vec3 *second);
struct KCR_Vec3 vec3_sub(struct KCR_Vec3 *first, struct KCR_Vec3 *second);
struct KCR_Vec3 vec3_mult(struct KCR_Vec3 *vector, float scalar);
struct KCR_Vec3 vec3_div(struct KCR_Vec3 *vector, float scalar);
float vec3_dot(struct KCR_Vec3 *first, struct KCR_Vec3 *second);
struct KCR_Vec3 vec3_cross(struct KCR_Vec3 *first, struct KCR_Vec3 *second);
struct KCR_Vec3 vec3_rotate_x(struct KCR_Vec3 *vector, float angle);
struct KCR_Vec3 vec3_rotate_y(struct KCR_Vec3 *vector, float angle);
struct KCR_Vec3 vec3_rotate_z(struct KCR_Vec3 *vector, float angle);

#endif //CRENDERER_VECTOR_H
