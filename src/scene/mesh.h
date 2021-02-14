#ifndef CRENDERER_MESH_H
#define CRENDERER_MESH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../math/vector.h"
#include "../math/matrix.h"

/*
 * Represents information about a single vertex within a mesh.  All indices are zero based, and dependent on their
 * position in the owning mesh's lists.
 */
struct KCR_Vertex {
    int positionIndex;
    int normalIndex;
    int textureIndex;
};

/*
 * Structure representing a face of a triangle, with the indexes to the vertex.  Faces are assumed to be
 * declared with a clockwise ordering.
 */
struct KCR_Face {
    struct KCR_Vertex v1;
    struct KCR_Vertex v2;
    struct KCR_Vertex v3;
    uint32_t color;
};

/*
 * Represents a 3d object, containing various aspects needed for rendering a mesh.
 */
struct KCR_Mesh {
    struct KCR_Vec3* vertexList;
    struct KCR_Face* faceList;
    struct KCR_Vec3* normalList;
    struct KCR_Vec2* textureCoordsList;
};

/*
 * A single instance of a mesh.  Allows rendering a single mesh multiple times with each instance
 * having it's own transforms and positions defined
 */
struct KCR_MeshInstance {
    struct KCR_Mesh* mesh;
    struct KCR_Vec3 position;
    struct KCR_Vec3 rotation;
    struct KCR_Vec3 scale;
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
void kcr_mesh_instance_uninit(struct KCR_MeshInstance* instance);

#endif //CRENDERER_MESH_H
