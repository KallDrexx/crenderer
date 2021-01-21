#ifndef CRENDERER_SCENE_H
#define CRENDERER_SCENE_H

#include "../gfx/display.h"
#include "../input.h"

struct KCR_Scene_Internal;
struct KCR_Scene {
    struct KCR_Scene_Internal* internal;
};

struct KCR_Scene* kcr_scene_init(void);
void kcr_scene_update(const struct KCR_Scene* scene);
void kcr_scene_render(struct KCR_Scene* scene, struct KCR_Display* display);
void kcr_scene_free(struct KCR_Scene* scene);

#endif //CRENDERER_SCENE_H
