#ifndef CRENDERER_VECTOR_H
#define CRENDERER_VECTOR_H

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

float vec3_length(Vec3 *vector);
Vec3 vec3_add(Vec3 *first, Vec3 *second);
Vec3 vec3_sub(Vec3 *first, Vec3 *second);
Vec3 vec3_mult(Vec3 *vector, float scalar);
Vec3 vec3_div(Vec3 *vector, float scalar);
float vec3_dot(Vec3 *first, Vec3 *second);
Vec3 vec3_cross(Vec3 *first, Vec3 *second);

#endif //CRENDERER_VECTOR_H
