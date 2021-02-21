#ifndef CRENDERER_TRIANGLE_RENDERER_H
#define CRENDERER_TRIANGLE_RENDERER_H

#include "display.h"
#include "render_settings.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../scene/lighting.h"
#include "../scene/texture.h"

/*
 * Represents a triangle that has been transformed
 */
struct KCR_RenderTriangle {
    struct KCR_Vec3 vertexPositions[3];
    struct KCR_Vec3 vertexNormals[3];
    struct KCR_Vec2 vertexTextureCoordinates[3];
    uint32_t color;
    float averageDepth;
    struct KCR_Vec3 normalizedTriangleNormal;
    struct KCR_Texture* texture;
};

void render_triangle(const struct KCR_Display* display,
                        const struct KCR_RenderSettings* renderSettings,
                        const struct KCR_RenderTriangle* triangle,
                        const struct KCR_GlobalLight* globalLight,
                        const struct KCR_Matrix4* projection);

#endif //CRENDERER_TRIANGLE_RENDERER_H
