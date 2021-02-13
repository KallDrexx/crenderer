#ifndef CRENDERER_TRIANGLE_RENDERER_H
#define CRENDERER_TRIANGLE_RENDERER_H

#include "display.h"
#include "render_settings.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../scene/lighting.h"

/*
 * Represents a triangle that has been transformed and projected
 */
struct KCR_RenderTriangle {
    struct KCR_Vec3 v1;
    struct KCR_Vec3 v2;
    struct KCR_Vec3 v3;
    uint32_t color;
    float averageDepth;
    struct KCR_Vec3 normalizedNormal;
};

void render_triangle(const struct KCR_Display* display,
                        const struct KCR_RenderSettings* renderSettings,
                        const struct KCR_RenderTriangle* triangle,
                        const struct KCR_GlobalLight* globalLight,
                        const struct KCR_Matrix4* projection);

#endif //CRENDERER_TRIANGLE_RENDERER_H
