#ifndef CRENDERER_SCENE_H
#define CRENDERER_SCENE_H

#include "../gfx/display.h"
#include "../input.h"
#include "mesh.h"

struct KCR_Scene {
    struct KCR_Mesh* mesh;
    struct KCR_Vec3 cameraPosition;
};

struct KCR_Scene* kcr_scene_create(void);
void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState,  float timeDelta);
void kcr_scene_free(struct KCR_Scene* scene);

#endif //CRENDERER_SCENE_H
