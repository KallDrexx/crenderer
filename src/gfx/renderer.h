#ifndef CRENDERER_RENDERER_H
#define CRENDERER_RENDERER_H

#include <stdbool.h>
#include "display.h"
#include "../scene/scene.h"
#include "../scene/mesh.h"
#include "../input.h"

struct KCR_RenderSettings {
    bool showVertices : 1;
    bool showSolidFaces : 1;
    bool showWireframe : 1;
    bool enableBackFaceCulling : 1;
};

struct KCR_RenderTriangle {
    struct KCR_Vec3 v1;
    struct KCR_Vec3 v2;
    struct KCR_Vec3 v3;
    uint32_t color;
};

/*
 * Represents the state of the renderer
 */
struct KCR_Renderer {
    const struct KCR_Display* display;
    struct KCR_RenderSettings renderMode;
    struct KCR_RenderTriangle* triangles;
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
