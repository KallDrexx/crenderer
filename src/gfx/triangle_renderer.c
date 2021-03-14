#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "triangle_renderer.h"
#include "../list.h"

#define SWAP(FIRST, SECOND, TYPE) do {TYPE SWAP = FIRST; FIRST = SECOND; SECOND = SWAP;} while(0)

struct RenderOperation {
    const struct KCR_Display* display;
    const struct KCR_RenderSettings* renderSettings;
    const struct KCR_RenderTriangle* triangle;
    const struct KCR_Vec4 projectedPoints[3];
    float* zBuffer;
};

struct BarycentricCoords {
    float w0;
    float w1;
    float w2;
};

struct Boundary {
    int minX;
    int minY;
    int maxX;
    int maxY;
};

/*
 * Gets the value of a color when multiplied by a value.
 */
uint32_t modify_color(uint32_t original, float multiplier) {
    assert(!isnan(multiplier));
    assert(!isinf(multiplier));
    assert(multiplier >= 0.0f);
    assert(multiplier <= 1.0f);

    uint32_t a = 0xFF000000;
    uint32_t r = (original & 0x00FF0000) * multiplier; // NOLINT(cppcoreguidelines-narrowing-conversions)
    uint32_t g = (original & 0x0000FF00) * multiplier; // NOLINT(cppcoreguidelines-narrowing-conversions)
    uint32_t b = (original & 0x000000FF) * multiplier; // NOLINT(cppcoreguidelines-narrowing-conversions)

    return a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

static inline float getLightAlignment(const struct KCR_GlobalLight* light, const struct KCR_Vec3 unitVector) {
    #define MIN_LIGHT 0.1f

    float alignment = kcr_vec3_dot(&light->direction, &unitVector);

    if (alignment < MIN_LIGHT) {
        alignment = MIN_LIGHT;
    }

    return alignment;
}

static inline void draw_pixel(const struct KCR_Display* display, int x, int y, uint32_t color) {
    if (x >= 0 && x < display->windowWidth && y >= 0 && y < display->windowHeight) {
        int index = kcr_display_get_pixel_index(display, x, y);
        display->pixelBuffer[index] = color;
    }
}

struct KCR_Vec4 perform_projection(const struct KCR_Matrix4* projection, const struct KCR_Vec3* vec3) {
    struct KCR_Vec4 result = kcr_vec4_from_vec3(vec3, 1);
    result = kcr_mat4_vec4_mult(projection, &result);

    // perspective divide
    if (result.w != 0) {
        result.x /= result.w;
        result.y /= result.w;
    }

    return result;
}

void adjust_to_screen_space(const struct KCR_Display* display, struct KCR_Vec4* point) {
    float centerWidth = (float) display->windowWidth / 2.0f;
    float centerHeight = (float) display->windowHeight / 2.0f;

    point->x *= centerWidth;
    point->x += centerWidth;

    point->y *= -centerHeight; // invert for canvas
    point->y += centerHeight;
}

static inline struct Boundary get_boundary(const struct KCR_Vec4* v0, const struct KCR_Vec4* v1, const struct KCR_Vec4* v2) {
    struct Boundary result = {
        .minX = (int) v0->x,
        .maxX = (int) v0->x,
        .minY = (int) v0->y,
        .maxY = (int) v0->y,
    };

    if (result.minX > (int) v1->x) result.minX = (int) v1->x;
    if (result.minX > (int) v2->x) result.minX = (int) v2->x;

    if (result.maxX < (int) v1->x) result.maxX = (int) v1->x;
    if (result.maxX < (int) v2->x) result.maxX = (int) v2->x;

    if (result.minY > (int) v1->y) result.minY = (int) v1->y;
    if (result.minY > (int) v2->y) result.minY = (int) v2->y;

    if (result.maxY < (int) v1->y) result.maxY = (int) v1->y;
    if (result.maxY < (int) v2->y) result.maxY = (int) v2->y;

    return result;
}

static inline float calc_double_area(const struct KCR_Vec4* v1, const struct KCR_Vec4* v2, const struct KCR_Vec4* v3) {
    // Only consider the x and y components for area calculation, since we are using this as an area of a triangle
    // and a triangle is always on a flat plane.
    struct KCR_Vec4 v13 = kcr_vec4_sub(v3, v1);
    struct KCR_Vec4 v12 = kcr_vec4_sub(v2, v1);

    return v13.x * v12.y - v13.y * v12.x;
}

static inline struct BarycentricCoords get_barycentric_coords(const struct KCR_Vec4* v0,
                                                              const struct KCR_Vec4* v1,
                                                              const struct KCR_Vec4* v2,
                                                              const struct KCR_Vec4* point,
                                                              float v012area) {
    struct BarycentricCoords coords = {
            .w0 = calc_double_area(v1, v2, point) / v012area,
            .w1 = calc_double_area(v2, v0, point) / v012area,
            .w2 = calc_double_area(v0, v1, point) / v012area,
    };

    return coords;
}

void perform_render(const struct RenderOperation* renderOperation,
                    const struct KCR_GlobalLight* globalLight) {

    // Render using barycentric coordinates
    struct Boundary boundary = get_boundary(&renderOperation->projectedPoints[0],
                                            &renderOperation->projectedPoints[1],
                                            &renderOperation->projectedPoints[2]);

    float fullArea = calc_double_area(&renderOperation->projectedPoints[0],
                                      &renderOperation->projectedPoints[1],
                                      &renderOperation->projectedPoints[2]);

    // Precalculate u and v divided by w for perspective correction calculations
    float uDividedByW[3] = {
            renderOperation->triangle->vertexTextureCoordinates[0].x / renderOperation->projectedPoints[0].w,
            renderOperation->triangle->vertexTextureCoordinates[1].x / renderOperation->projectedPoints[1].w,
            renderOperation->triangle->vertexTextureCoordinates[2].x / renderOperation->projectedPoints[2].w,
    };

    float vDividedByW[3] = {
            renderOperation->triangle->vertexTextureCoordinates[0].y / renderOperation->projectedPoints[0].w,
            renderOperation->triangle->vertexTextureCoordinates[1].y / renderOperation->projectedPoints[1].w,
            renderOperation->triangle->vertexTextureCoordinates[2].y / renderOperation->projectedPoints[2].w,
    };

    float reciprocalW[3] = {
            1 / renderOperation->projectedPoints[0].w,
            1 / renderOperation->projectedPoints[1].w,
            1 / renderOperation->projectedPoints[2].w,
    };

    for (int x = boundary.minX; x <= boundary.maxX; x++) {
        for (int y = boundary.minY; y <= boundary.maxY; y++) {
            if (x < 0 || x >= renderOperation->display->windowWidth || y < 0 || y >= renderOperation->display->windowHeight) {
                continue;
            }

            struct KCR_Vec4 point = {.x = (float) x, .y = (float) y, .z = 0, .w = 0};

            struct BarycentricCoords coords = get_barycentric_coords(&renderOperation->projectedPoints[0],
                                                                     &renderOperation->projectedPoints[1],
                                                                     &renderOperation->projectedPoints[2],
                                                                     &point,
                                                                     fullArea);

            // Only inside the triangle if all 3 are zero or positive
            if (coords.w0 < 0 || coords.w1 < 0 || coords.w2 < 0) {
                continue;
            }

            // Calculate the reciprocal of w's barycentric coordinates so we are able to figure the actual depth
            // of this pixel, for checking against the z buffer.  We need to use the w value as that contains the
            // pre-projected depth/z value of the transformed triangle, and thus this is required for perspective
            // correct interpolation.
            float interpolatedReciprocalW = reciprocalW[0] * coords.w0 + reciprocalW[1] * coords.w1 + reciprocalW[2] * coords.w2;

            int index = kcr_display_get_pixel_index(renderOperation->display, x, y);
            if (renderOperation->zBuffer[index] <= interpolatedReciprocalW) {
                // Already drew a pixel from a triangle that's closer
                continue;
            }

            renderOperation->zBuffer[index] = interpolatedReciprocalW;

            bool pixelDrawn = false;
            if (renderOperation->renderSettings->showWireframe) {
                // are we on a line?
                // WARNING: This creates gaps thick lines for some lines and gaps in other lines.  It's not clear if
                // we are able to use barycentric coordinates reliably for line rendering, and may have to convert to
                // a bresenham's line algorithm after all pixels have been drawn.  We may be able to do this by only
                // allowing negative values if they are sufficiently small, but don't use negative numbers for triangle
                // filling, only for line rendering.  For now this works.
                if (coords.w0 < 0.01f || coords.w1 < 0.01f || coords.w2 < 0.01f) {
                    pixelDrawn = true;

                    // Draw black if we are filling pixels in, otherwise draw white
                    uint32_t color = renderOperation->renderSettings->triangleFillMode == FILL_NONE
                            ? 0xFFFFFFFF
                            : 0xFF000000;

                    draw_pixel(renderOperation->display, x, y, color);
                }
            }

            if (!pixelDrawn && renderOperation->renderSettings->triangleFillMode != FILL_NONE) {
                uint32_t color = 0xFF000000;
                switch (renderOperation->renderSettings->triangleFillMode) {
                    case FILL_NONE:
                        break; // make linter happy

                    case FILL_WHITE:
                        color = 0xFFFFFFFF;
                        break;

                    case FILL_MESH_TRI_COLORS:
                        color = renderOperation->triangle->color;
                        break;

                    case FILL_TEXTURED: {
                        // perspective correct interpolation for texture coords based on barycentric coordinates
                        float u, v;

                        // Divide each u and v value by their corresponding w (original triangle Z depth) in order to
                        // account for perspective.
                        u = uDividedByW[0] * coords.w0 + uDividedByW[1] * coords.w1 + uDividedByW[2] * coords.w2;
                        v = vDividedByW[0] * coords.w0 + vDividedByW[1] * coords.w1 + vDividedByW[2] * coords.w2;

                        // Since we accounted for perspective by dividing the u and v components by w, we need to divide
                        // u and v by the interpolated reciprocal of w to undo the perspective divide.
                        u /= interpolatedReciprocalW;
                        v /= interpolatedReciprocalW;

                        uint32_t index = kcr_texture_texel_index(renderOperation->triangle->texture, u, v);
                        color = renderOperation->triangle->texture->texels[index];
                        break;
                    }
                }

                switch(renderOperation->renderSettings->lightingMode) {
                    case LIGHTING_NONE:
                        break;

                    case LIGHTING_FLAT: {
                        float alignment = getLightAlignment(globalLight, renderOperation->triangle->normalizedTriangleNormal);
                        color = modify_color(color, alignment);
                        break;
                    }

                    case LIGHTING_SMOOTH: {
                        struct KCR_Vec3 normal = {
                                .x = renderOperation->triangle->vertexNormals[0].x * coords.w0 +
                                     renderOperation->triangle->vertexNormals[1].x * coords.w1 +
                                     renderOperation->triangle->vertexNormals[2].x * coords.w2,

                                .y = renderOperation->triangle->vertexNormals[0].y * coords.w0 +
                                      renderOperation->triangle->vertexNormals[1].y * coords.w1 +
                                      renderOperation->triangle->vertexNormals[2].y * coords.w2,

                                .z = renderOperation->triangle->vertexNormals[0].z * coords.w0 +
                                     renderOperation->triangle->vertexNormals[1].z * coords.w1 +
                                     renderOperation->triangle->vertexNormals[2].z * coords.w2,
                        };

                        float alignment = getLightAlignment(globalLight, normal);
                        color = modify_color(color, alignment);
                        break;
                    }
                }

                draw_pixel(renderOperation->display, x, y, color);
            }
        }
    }
}

void render_triangle(const struct KCR_Display* display,
                     const struct KCR_RenderSettings* renderSettings,
                     const struct KCR_RenderTriangle* triangle,
                     const struct KCR_GlobalLight* globalLight,
                     const struct KCR_Matrix4* projection,
                     float* zBuffer) {
    struct KCR_Vec4 projectedVector1 = perform_projection(projection, &triangle->vertexPositions[0]);
    struct KCR_Vec4 projectedVector2 = perform_projection(projection, &triangle->vertexPositions[1]);
    struct KCR_Vec4 projectedVector3 = perform_projection(projection, &triangle->vertexPositions[2]);

    // Since we are using perspective correction, this means that a triangle normal in 3d space may be facing away
    // from the camera, but is still visible in the periphery of our view.  This means if we have to do backface
    // culling *after* perspective projection occurred.  While this gives us a flat screen space triangle, it will
    // either be facing directly towards or away from the eye.  So we can use the cross product to know which.
    struct KCR_Vec3 pv1 = kcr_vec3_from_vec4(&projectedVector1);
    struct KCR_Vec3 pv2 = kcr_vec3_from_vec4(&projectedVector2);
    struct KCR_Vec3 pv3 = kcr_vec3_from_vec4(&projectedVector3);
    struct KCR_Vec3 cv1 = kcr_vec3_sub(&pv2, &pv1);
    struct KCR_Vec3 cv2 = kcr_vec3_sub(&pv3, &pv1);
    struct KCR_Vec3 cross = kcr_vec3_cross(&cv1, &cv2);
    if (!renderSettings->enableBackFaceCulling && cross.z > 0) {
        return;
    }

    adjust_to_screen_space(display, &projectedVector1);
    adjust_to_screen_space(display, &projectedVector2);
    adjust_to_screen_space(display, &projectedVector3);

    // Ignore triangles outside screen boundaries
    float maxY = (float) display->windowHeight;
    float maxX = (float) display->windowWidth;
    if (projectedVector1.y < 0 && projectedVector2.y < 0 && projectedVector3.y < 0) return;
    if (projectedVector1.y > maxY && projectedVector2.y > maxY && projectedVector3.y > maxY) return;
    if (projectedVector1.x < 0 && projectedVector2.x < 0 && projectedVector3.x < 0) return;
    if (projectedVector1.x > maxX && projectedVector2.x > maxX && projectedVector3.x > maxX) return;

    struct RenderOperation renderOperation = {
        .display = display,
        .triangle = triangle,
        .renderSettings = renderSettings,
        .projectedPoints = {projectedVector1, projectedVector2, projectedVector3},
        .zBuffer = zBuffer,
    };

    perform_render(&renderOperation, globalLight);
}
