#ifndef CRENDERER_SCENE_H
#define CRENDERER_SCENE_H

#include "../gfx/display.h"
#include "../input.h"

void scene_init(void);
void scene_update(struct KCR_InputState* inputState);
void scene_render(struct KCR_Display* display);

#endif //CRENDERER_SCENE_H
