#include <malloc.h>
#include <mem.h>
#include "mesh.h"

struct KCR_Vec3 cubeVectors[] = {
    {1, 1, -1}, // front top right
    {1, -1, -1}, // front bottom right
    {-1, -1, -1}, // front bottom left
    {-1, 1, -1}, // front top left
    {1, 1, 1}, // back top right
    {1, -1, 1}, // back bottom right
    {-1, -1, 1}, // back bottom left
    {-1, 1, 1}, // back top left
};

struct KCR_Face cubeFaces[] = {
    {2, 3, 0}, // front-top
    {2, 0, 1}, // front-bottom
    {1, 0, 4}, // right-top
    {1, 4, 5}, // right-bottom
    {2, 7, 3}, // left-top
    {2, 6, 7}, // left-bottom
    {2, 1, 5}, // bottom-forward
    {2, 5, 6}, // bottom-rear
    {3, 4, 0}, // top-forward
    {3, 7, 3}, // rop-rear
    {5, 4, 7}, // back-top
    {5, 7, 6}, // back-bottom
};

struct KCR_Mesh* init_mesh_struct(int vertexCount, int faceCount) {
    struct KCR_Mesh* mesh = malloc(sizeof(struct KCR_Mesh));
    mesh->verticesCount = vertexCount;
    mesh->faceCount = faceCount;

    mesh->vertices = malloc(sizeof(struct KCR_Vec3) * vertexCount);
    mesh->faces = malloc(sizeof(struct KCR_Face) * faceCount);

    return mesh;
}

struct KCR_Mesh* kcr_mesh_create_cube(void) {
    struct KCR_Mesh* mesh = init_mesh_struct(8, 12);
    memcpy(mesh->vertices, cubeVectors, sizeof(cubeVectors));
    memcpy(mesh->faces, cubeFaces, sizeof(cubeFaces));

    return mesh;
}

void kcr_mesh_free(struct KCR_Mesh *mesh) {
    if (mesh != NULL) {
        free(mesh->faces);
        free(mesh->vertices);
    }

    free(mesh);
}
