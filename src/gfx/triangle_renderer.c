#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "triangle_renderer.h"
#include "../list.h"

#define SWAP(FIRST, SECOND, TYPE) do {TYPE SWAP = FIRST; FIRST = SECOND; SECOND = SWAP;} while(0)

struct LineYPoint {
    int y;
    int minX;
    int maxX;
};

struct ScanLineRenderOperation {
    const struct KCR_Display* display;
    const struct KCR_RenderSettings* renderSettings;
    const struct KCR_Texture* texture;
    struct LineYPoint lineLeft;
    struct LineYPoint lineRight;
    uint32_t defaultTriangleColor;
    int yCoord;
    float leftLightAlignment;
    float rightLightAlignment;
    float faceLightAlignment;
    struct KCR_Vec2 leftTextureCoords;
    struct KCR_Vec2 rightTextureCoords;
};

// Re-used point lists so we don't have to allocate every triangle
struct LineYPoint* topToMidPoints;
struct LineYPoint* topToBottomPoints;
struct LineYPoint* midToBottomPoints;

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

/*
 * Linear interpolation between two values
 */
static inline float interpolate(float first, float second, float percent) {
    if (first < second) {
        SWAP(first, second, float);
        percent = 1 - percent;
    }

    return first - (first - second) * percent;
}

/*
 * Linear interpolation between two 2 component vectors
 */
static inline struct KCR_Vec2 interpolate_vec2(struct KCR_Vec2 first, struct KCR_Vec2 second, float percent) {
    float x = interpolate(first.x, second.x, percent);
    float y = interpolate(first.y, second.y, percent);

    return (struct KCR_Vec2){x, y};
}

int sort_points(const void* a, const void* b) {
    struct LineYPoint* pointA = (struct LineYPoint*) a;
    struct LineYPoint* pointB = (struct LineYPoint*) b;

    return pointA->y - pointB->y;
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

void calc_line_y_points(int x1, int y1, int x2, int y2, struct LineYPoint** pointList) {
    assert(pointList != NULL);

    // Brensenham requires slope between 0 and 1, so normalize for that
    bool steep = abs(y2 - y1) > abs(x2 - x1);
    if (steep) {
        SWAP(x1, y1, int);
        SWAP(x2, y2, int);
    }

    if (x1 > x2) {
        SWAP(x1, x2, int);
        SWAP(y1, y2, int);
    }

    int changeInX = x2 - x1;
    int absChangeInY = abs(y2 - y1);
    int error = changeInX / 2;
    int yStep = y1 < y2 ? 1 : -1;
    int y = y1;

    if (*pointList == NULL) {
        *pointList = kcr_list_create(sizeof(struct LineYPoint));
    }
    else {
        kcr_list_clear(*pointList);
    }

    struct LineYPoint* currentPoint = NULL;

    for (int x = x1; x <= x2; x++) {
        int drawX = steep ? y : x;
        int drawY = steep ? x : y;

        if (currentPoint == NULL || currentPoint->y != drawY) {
            currentPoint = kcr_list_new_item((void**) pointList);
            currentPoint->minX = drawX;
            currentPoint->maxX = drawX;
            currentPoint->y = drawY;
        }
        else {
            if (currentPoint->minX > drawX) currentPoint->minX = drawX;
            if (currentPoint->maxX < drawX) currentPoint->maxX = drawX;
        }

        error = error - absChangeInY;
        if (error < 0) {
            y += yStep;
            error += changeInX;
        }
    }

    qsort(*pointList, kcr_list_length(*pointList), sizeof(struct LineYPoint), sort_points);
}

struct KCR_Vec2 adjust_to_screen_space(const struct KCR_Display* display, struct KCR_Vec2 point) {
    float centerWidth = (float) display->windowWidth / 2.0f;
    float centerHeight = (float) display->windowHeight / 2.0f;

    point.x *= centerWidth;
    point.x += centerWidth;

    point.y *= -centerHeight; // invert for canvas
    point.y += centerHeight;

    return point;
}

void render_scanline(const struct ScanLineRenderOperation* renderOperation) {
    if (renderOperation->lineLeft.minX > renderOperation->display->windowWidth || renderOperation->lineRight.maxX < 0) {
        // Out of the window, so don't bother rendering
        return;
    }

    int left = renderOperation->lineLeft.minX;
    int right = renderOperation->lineRight.maxX;
    if (left < 0) left = 0;
    if (right > renderOperation->display->windowWidth) right = renderOperation->display->windowWidth;

    for (int x = left; x <= right; x++) {
        bool pixelRenderingSet = false;
        uint32_t pixelColor = 0xFFFFFFFF;

        if (renderOperation->renderSettings->showWireframe) {
            if ((x >= renderOperation->lineLeft.minX && x <= renderOperation->lineLeft.maxX) ||
                (x >= renderOperation->lineRight.minX && x <= renderOperation->lineRight.maxX)) {
                pixelColor = renderOperation->renderSettings->triangleFillMode != FILL_NONE ? 0xFF000000 : 0xFFFFFFFF;
                pixelRenderingSet = true;
            }
        }

        if (renderOperation->renderSettings->triangleFillMode != FILL_NONE && !pixelRenderingSet) {
            pixelRenderingSet = true;

            // Set the pixel color
            switch (renderOperation->renderSettings->triangleFillMode) {
                case FILL_NONE:
                    assert(false); // should never occur
                    break;

                case FILL_MESH_TRI_COLORS:
                    pixelColor = renderOperation->defaultTriangleColor;
                    break;

                case FILL_WHITE:
                    pixelColor = 0xFFFFFFFF;
                    break;

                case FILL_TEXTURED: {
                    struct KCR_Vec2 textureCoords;
                    if (right == left) {
                        textureCoords = renderOperation->leftTextureCoords;
                    } else {
                        float progress = (float) (x - left) / (float) (right - left);
                        textureCoords = interpolate_vec2(renderOperation->leftTextureCoords,
                                                         renderOperation->rightTextureCoords,
                                                         progress);
                    }

                    uint32_t index = kcr_texture_texel_index(renderOperation->texture, textureCoords.x, textureCoords.y);
                    pixelColor = renderOperation->texture->texels[index];

                    break;
                }
            }

            switch (renderOperation->renderSettings->lightingMode) {
                case LIGHTING_NONE:
                    // No change
                    break;

                case LIGHTING_FLAT:
                    pixelColor = modify_color(pixelColor, renderOperation->faceLightAlignment);
                    break;

                case LIGHTING_SMOOTH: {
                    float alignment;
                    if (right == left) {
                        alignment = renderOperation->leftLightAlignment;
                    } else {
                        float length = (float) (x - left) / (float) (right - left);
                        alignment = interpolate(renderOperation->leftLightAlignment, renderOperation->rightLightAlignment, length);
                    }

                    pixelColor = modify_color(pixelColor, alignment);

                    break;
                }
            }
        }

        if (pixelRenderingSet) {
            draw_pixel(renderOperation->display, x, renderOperation->yCoord, pixelColor);
        }
    }
}

void perform_render(const struct KCR_Display* display,
                    const struct KCR_RenderSettings *renderSettings,
                    const struct KCR_RenderTriangle* triangle,
                    const struct KCR_GlobalLight *globalLight,
                    const struct KCR_Vec2 points[3]) {
    int topIndex = 0,
            midIndex = 1,
            bottomIndex = 2;

    if (points[topIndex].y > points[midIndex].y) SWAP(topIndex, midIndex, int);
    if (points[midIndex].y > points[bottomIndex].y) SWAP(midIndex, bottomIndex, int);
    if (points[topIndex].y > points[midIndex].y) SWAP(topIndex, midIndex, int);

    // Get points for all 3 sets of lines, so we know the boundaries for each scanline
    calc_line_y_points((int) points[topIndex].x, (int) points[topIndex].y, (int) points[midIndex].x, (int) points[midIndex].y, &topToMidPoints);
    calc_line_y_points((int) points[topIndex].x, (int) points[topIndex].y, (int) points[bottomIndex].x, (int) points[bottomIndex].y, &topToBottomPoints);
    calc_line_y_points((int) points[midIndex].x, (int) points[midIndex].y, (int) points[bottomIndex].x, (int) points[bottomIndex].y, &midToBottomPoints);

    size_t topToMidCount = kcr_list_length(topToMidPoints);
    size_t topToBottomCount = kcr_list_length(topToBottomPoints);
    size_t midToBottomCount = kcr_list_length(midToBottomPoints);

    assert(topToBottomCount >= 1);
    assert(topToMidCount <= topToBottomCount);
    assert(topToMidCount + midToBottomCount == topToBottomCount + 1); // +1 for overlap
    assert(midToBottomPoints[midToBottomCount - 1].y == topToBottomPoints[topToBottomCount - 1].y);

    int startY = topToBottomPoints[0].y;
    int endY = topToBottomPoints[topToBottomCount - 1].y;
    size_t topToMidIndex = 0;
    size_t topToBottomIndex = 0;
    size_t midToBottomIndex = 0;
    bool reachedMidPoint = false;

    float faceLightAlignment = getLightAlignment(globalLight, triangle->normalizedTriangleNormal);
    float topAlignment = getLightAlignment(globalLight, triangle->vertexNormals[topIndex]);
    float midAlignment = getLightAlignment(globalLight, triangle->vertexNormals[midIndex]);
    float bottomAlignment = getLightAlignment(globalLight, triangle->vertexNormals[bottomIndex]);

    struct KCR_Vec2 topTextureCoords = triangle->vertexTextureCoordinates[topIndex];
    struct KCR_Vec2 midTextureCoords = triangle->vertexTextureCoordinates[midIndex];
    struct KCR_Vec2 bottomTextureCoords = triangle->vertexTextureCoordinates[bottomIndex];

    struct ScanLineRenderOperation renderOp = {
            .display = display,
            .renderSettings = renderSettings,
            .texture = triangle->texture,
            .defaultTriangleColor = triangle->color,
            .faceLightAlignment = faceLightAlignment,
    };

    for (int y = startY; y <= endY; y++) {
        struct LineYPoint left = topToBottomPoints[topToBottomIndex];
        struct LineYPoint right = reachedMidPoint
                ? midToBottomPoints[midToBottomIndex]
                : topToMidPoints[topToMidIndex];

        float leftAlignment;
        struct KCR_Vec2 leftTextureCoords;
        if (endY - startY > 0) {
            float progress = (float) (y - startY) / (float) (endY - startY);
            leftAlignment = interpolate(topAlignment, bottomAlignment, progress);
            leftTextureCoords = interpolate_vec2(topTextureCoords, bottomTextureCoords, progress);
        } else {
            leftAlignment = topAlignment;
            leftTextureCoords = topTextureCoords;
        }

        float rightAlignment;
        struct KCR_Vec2 rightTextureCoords;
        int rightTotal = reachedMidPoint ? endY - midToBottomPoints[0].y : midToBottomPoints[0].y - startY;
        if (rightTotal > 0) {
            float progress = reachedMidPoint
                    ? (float) (y - midToBottomPoints[0].y) / (float) rightTotal
                    : (float) (y - topToMidPoints[0].y) / (float) rightTotal;

            rightAlignment = reachedMidPoint
                    ? interpolate(midAlignment, bottomAlignment, progress)
                    : interpolate(topAlignment, midAlignment, progress);

            rightTextureCoords = reachedMidPoint
                    ? interpolate_vec2(midTextureCoords, bottomTextureCoords, progress)
                    : interpolate_vec2(topTextureCoords, midTextureCoords, progress);
        } else {
            rightAlignment = reachedMidPoint ? midAlignment : topAlignment;
            rightTextureCoords = reachedMidPoint ? midTextureCoords : topTextureCoords;
        }

        if (left.maxX > right.minX) {
            SWAP(left, right, struct LineYPoint);
            SWAP(leftAlignment, rightAlignment, float);
            SWAP(leftTextureCoords, rightTextureCoords, struct KCR_Vec2);
        }

        assert(left.y == y);
        assert(right.y == y);

        renderOp.yCoord = y;
        renderOp.lineLeft = left;
        renderOp.lineRight = right;
        renderOp.leftLightAlignment = leftAlignment;
        renderOp.rightLightAlignment = rightAlignment;
        renderOp.leftTextureCoords = leftTextureCoords;
        renderOp.rightTextureCoords = rightTextureCoords;

        render_scanline(&renderOp);

        if (!reachedMidPoint && topToMidIndex >= topToMidCount - 1) {
            reachedMidPoint = true;

            // Since top->mid and mid->bottom overlaps, we need to make sure we draw the overlap from both angles,
            // otherwise we will get holes in the lines.  I *think* this will only happen during wireframe rendering
            // so this may be ok with shading.
            y--;
            continue;
        }

        topToBottomIndex++;
        if (reachedMidPoint) {
            midToBottomIndex++;
        } else {
            topToMidIndex++;
        }
    }

    if (renderSettings->showVertices) {
        for (int i = 0; i < 3; i++) {
            #define RECT_SIZE 6.0f
            #define HALF_RECT RECT_SIZE / 2

            for (int x = (int)(points[i].x - HALF_RECT); x < (int)(points[i].x + HALF_RECT); x++) {
                for (int y = (int)(points[i].y - HALF_RECT); y < (int)(points[i].y + HALF_RECT); y++) {
                    draw_pixel(display, x, y, 0xFFFF0000);
                }
            }
        }
    }
}

void render_triangle(const struct KCR_Display* display,
                     const struct KCR_RenderSettings* renderSettings,
                     const struct KCR_RenderTriangle* triangle,
                     const struct KCR_GlobalLight* globalLight,
                     const struct KCR_Matrix4* projection) {
    struct KCR_Vec4 projectedVector1 = perform_projection(projection, &triangle->vertexPositions[0]);
    struct KCR_Vec4 projectedVector2 = perform_projection(projection, &triangle->vertexPositions[1]);
    struct KCR_Vec4 projectedVector3 = perform_projection(projection, &triangle->vertexPositions[2]);

    struct KCR_Vec2 projectedPoint1 = adjust_to_screen_space(display, kcr_vec2_from_vec4(&projectedVector1));
    struct KCR_Vec2 projectedPoint2 = adjust_to_screen_space(display, kcr_vec2_from_vec4(&projectedVector2));
    struct KCR_Vec2 projectedPoint3 = adjust_to_screen_space(display, kcr_vec2_from_vec4(&projectedVector3));

    // Ignore triangles outside screen boundaries
    float maxY = (float) display->windowHeight;
    float maxX = (float) display->windowWidth;
    if (projectedPoint1.y < 0 && projectedPoint2.y < 0 && projectedPoint3.y < 0) return;
    if (projectedPoint1.y > maxY && projectedPoint2.y > maxY && projectedPoint3.y > maxY) return;
    if (projectedPoint1.x < 0 && projectedPoint2.x < 0 && projectedPoint3.x < 0) return;
    if (projectedPoint1.x > maxX && projectedPoint2.x > maxX && projectedPoint3.x > maxX) return;

    struct KCR_Vec2 points[3] = {projectedPoint1, projectedPoint2, projectedPoint3};

    perform_render(display, renderSettings, triangle, globalLight, points);
}
