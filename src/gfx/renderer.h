#ifndef CRENDERER_RENDERER_H
#define CRENDERER_RENDERER_H

#include <stdbool.h>
#include "display.h"
#include "../scene/scene.h"
#include "../input.h"

struct KCR_RenderMode {
    bool showVertices : 1;
    bool showSolidFaces : 1;
    bool showWireframe : 1;
};

/*
 * Represents the state of the renderer
 */
struct KCR_Renderer {
    const struct KCR_Display* display;
    struct KCR_RenderMode renderMode;
};

/*
 * Initializes a new renderer
 */
bool kcr_renderer_init(struct KCR_Renderer* renderer, const struct KCR_Display* display);

/*
 * Uninitializes a renderer and releases memory it has allocated.  The renderer itself will not be freed
 */
void kcr_renderer_uninit(struct KCR_Renderer* renderer);

/*
 * Renders the current scene using the passed in initialized renderer
 */
void kcr_renderer_render(struct KCR_Renderer* renderer,
        const struct KCR_Scene* scene,
        const struct KCR_InputState* inputState);

#endif //CRENDERER_RENDERER_H
