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
    struct KCR_Vec3* vertexList;
    struct KCR_Face* faceList;
    struct KCR_Vec3 rotation;
};

struct KCR_Mesh* kcr_mesh_from_obj_file(char* filename);
void kcr_mesh_free(struct KCR_Mesh* mesh);

#endif //CRENDERER_MESH_H
