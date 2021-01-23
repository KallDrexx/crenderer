#ifndef CRENDERER_MESH_H
#define CRENDERER_MESH_H

#include <stddef.h>
#include "../math/vector.h"

struct KCR_Face {
    int vertexIndex1;
    int vertexIndex2;
    int vertexIndex3;
};

struct KCR_Mesh {
    int verticesCount;
    int faceCount;

    struct KCR_Vec3 *vertices;
    struct KCR_Face *faces;
};

struct KCR_Mesh* mesh_create_cube(float size);
void mesh_free(struct KCR_Mesh* mesh);

#endif //CRENDERER_MESH_H
