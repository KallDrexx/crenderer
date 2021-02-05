#ifndef CRENDERER_MESH_H
#define CRENDERER_MESH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../math/vector.h"
#include "../math/matrix.h"

/*
 * Structure representing a face of a triangle, with the indexes to the vertex.  Faces are assumed to be
 * declared with a clockwise ordering.
 */
struct KCR_Face {
    int vertexIndex1;
    int vertexIndex2;
    int vertexIndex3;
    uint32_t color;
};

/*
 * Represents a 3d object, containing all individual vertices and triangle faces its made up of
 */
struct KCR_Mesh {
    struct KCR_Vec3* vertexList;
    struct KCR_Face* faceList;
};

/*
 * A single instance of a mesh.  Allows rendering a single mesh multiple times with each instance
 * having it's own transforms and positions defined
 */
struct KCR_MeshInstance {
    struct KCR_Mesh* mesh;
    struct KCR_Vec3 position;
    struct KCR_Matrix4 transform;
};

/*
 * Initializes a not-yet initialized mesh from a file in the Wavefront OBJ file format.
 */
bool kcr_mesh_from_obj_file(struct KCR_Mesh* mesh, char* filename);

/*
 * Un-initializes a mesh, freeing memory for pointers it controls.  The mesh itself will not be deallocated
 */
void kcr_mesh_uninit(struct KCR_Mesh* mesh);

/*
 * Initializes a not yet initialized mesh instance and assign it's starting mesh.
 */
bool kcr_mesh_instance_init(struct KCR_MeshInstance* instance, struct KCR_Mesh* mesh);

/*
 * Un-initializes a mesh instance.  This will not deallocate the mesh the instance is pointing to
 * in case multiple instances are using the same mesh.  The instance itself will not be deallocated
 */
__attribute__((unused)) void kcr_mesh_instance_uninit(__attribute__((unused)) struct KCR_MeshInstance* instance);

#endif //CRENDERER_MESH_H
