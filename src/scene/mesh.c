#include <malloc.h>
#include "mesh.h"

struct KCR_Mesh* init_mesh_struct(int verticeCount, int faceCount) {
    struct KCR_Mesh* mesh = malloc(sizeof(struct KCR_Mesh));
    mesh->verticesCount = verticeCount;
    mesh->faceCount = faceCount;

    mesh->vertices = malloc(sizeof(struct KCR_Vec3) * verticeCount);
    mesh->faces = malloc(sizeof(struct KCR_Face) * faceCount);

    return mesh;
}

struct KCR_Mesh* mesh_create_cube(float size) {
    struct KCR_Vec3 v0 = {size, size, -size}; // front top right
    struct KCR_Vec3 v1 = {size, -size, -size}; // front bottom right
    struct KCR_Vec3 v2 = {-size, -size, -size}; // front bottom left
    struct KCR_Vec3 v3 = {-size, size, -size}; // front top left
    struct KCR_Vec3 v4 = {size, size, size}; // back top right
    struct KCR_Vec3 v5 = {size, -size, size}; // back bottom right
    struct KCR_Vec3 v6 = {-size, -size, size}; // back bottom left
    struct KCR_Vec3 v7 = {-size, size, size}; // back top left

    struct KCR_Face f0 = {2, 3, 0}; // front-top
    struct KCR_Face f1 = {2, 0, 1}; // front-bottom
    struct KCR_Face f2 = {1, 0, 4}; // right-top
    struct KCR_Face f3 = {1, 4, 5}; // right-bottom
    struct KCR_Face f4 = {2, 7, 3}; // left-top
    struct KCR_Face f5 = {2, 6, 7}; // left-bottom
    struct KCR_Face f6 = {2, 1, 5}; // bottom-forward
    struct KCR_Face f7 = {2, 5, 6}; // bottom-rear
    struct KCR_Face f8 = {3, 4, 0}; // top-forward
    struct KCR_Face f9 = {3, 7, 3}; // rop-rear
    struct KCR_Face f10 = {5, 4, 7}; // back-top
    struct KCR_Face f11 = {5, 7, 6}; // back-bottom

    struct KCR_Mesh* mesh = init_mesh_struct(8, 12);
    mesh->vertices[0] = v0;
    mesh->vertices[1] = v1;
    mesh->vertices[2] = v2;
    mesh->vertices[3] = v3;
    mesh->vertices[4] = v4;
    mesh->vertices[5] = v5;
    mesh->vertices[6] = v6;
    mesh->vertices[7] = v7;
    mesh->faces[0] = f0;
    mesh->faces[1] = f1;
    mesh->faces[2] = f2;
    mesh->faces[3] = f3;
    mesh->faces[4] = f4;
    mesh->faces[5] = f5;
    mesh->faces[6] = f6;
    mesh->faces[7] = f7;
    mesh->faces[8] = f8;
    mesh->faces[9] = f9;
    mesh->faces[10] = f10;
    mesh->faces[11] = f11;

    return mesh;
}

void mesh_free(struct KCR_Mesh *mesh) {
    if (mesh != NULL) {
        free(mesh->faces);
        free(mesh->vertices);
    }

    free(mesh);
}
