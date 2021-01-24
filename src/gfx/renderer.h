#ifndef CRENDERER_RENDERER_H
#define CRENDERER_RENDERER_H

#include "display.h"
#include "../scene/scene.h"

void kcr_render(const struct KCR_Display* display, const struct KCR_Scene* scene);

#endif //CRENDERER_RENDERER_H