#ifndef CRENDERER_SCENE_H
#define CRENDERER_SCENE_H

#include "../gfx/display.h"
#include "../input.h"
#include "mesh.h"

/*
 * Represents a camera in the scene
 */
struct KCR_Camera {
    struct KCR_Vec3 position;
    float fieldOfViewRadians;
    float zNear;
    float zFar;
};

/*
 * Global illumination light
 */
struct KCR_GlobalLight {
    struct KCR_Vec3 direction;
};

/*
 * Structure representing all objects in the scene, including the camera.
 */
struct KCR_Scene {
    struct KCR_Mesh* meshList;
    struct KCR_MeshInstance* instanceList;
    struct KCR_Camera camera;
    struct KCR_GlobalLight globalLight;
};

/*
 * Initializes a not-yet initialized instance of a scene with default values.  If a scene is passed in
 * that has been initialized but not uninitialized than memory leaks will occur.
 */
bool kcr_scene_init(struct KCR_Scene* scene);

/*
 * Updates the scene for the current frame based on the inputs the user has made and the time since
 * the last frame.
 */
void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState, float timeDelta);

/*
 * Un-initializes the scene, freeing any memory the scene itself might have allocated.  The scene itself
 * will not be freed.
 */
void kcr_scene_uninit(struct KCR_Scene* scene);

#endif //CRENDERER_SCENE_H
