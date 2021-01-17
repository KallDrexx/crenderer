#ifndef CRENDERER_VECTOR_H
#define CRENDERER_VECTOR_H

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    float x;
    float y;
    float z;
} Vector3;

float vector3_length(Vector3* vector);
Vector3 vector3_add(Vector3* first, Vector3* second);
Vector2 project_to_2d(Vector3* vector);

float vector2_length(Vector2* vector);
Vector2 vector2_add(Vector2* first, Vector2* second);

#endif //CRENDERER_VECTOR_H
