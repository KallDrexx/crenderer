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

// Re-used point lists so we don't have to allocate every triangle
struct LineYPoint* topToMidPoints;
struct LineYPoint* topToBottomPoints;
struct LineYPoint* midToBottomPoints;

/*
 * Gets the value of a color when multiplied by a value.
 */
static inline uint32_t modify_color(uint32_t original, float multiplier) {
    assert(multiplier >= 0);
    assert(multiplier <= 1.0f);

    uint32_t a = 0xFF000000;
    uint32_t r = (original & 0x00FF0000) * multiplier; // NOLINT(cppcoreguidelines-narrowing-conversions)
    uint32_t g = (original & 0x0000FF00) * multiplier; // NOLINT(cppcoreguidelines-narrowing-conversions)
    uint32_t b = (original & 0x000000FF) * multiplier; // NOLINT(cppcoreguidelines-narrowing-conversions)

    return a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

static inline void draw_pixel(const struct KCR_Display* display, int x, int y, uint32_t color) {
    if (x >= 0 && x < display->windowWidth && y >= 0 && y < display->windowHeight) {
        int index = kcr_display_get_pixel_index(display, x, y);
        display->pixelBuffer[index] = color;
    }
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

    point.x *= -centerWidth; // I'm not quite sure why the x axis is flipped
    point.x += centerWidth;

    point.y *= -centerHeight; // invert for canvas
    point.y += centerHeight;

    return point;
}

void render_scanline(const struct KCR_Display* display,
        const struct KCR_RenderSettings* renderSettings,
        const struct KCR_RenderTriangle* triangle,
        const struct KCR_GlobalLight* globalLight,
        const struct LineYPoint lineLeft,
        const struct LineYPoint lineRight) {

    if (lineLeft.minX > display->windowWidth || lineRight.maxX < 0) {
        // Out of the window, so don't bother rendering
        return;
    }

    if (renderSettings->showSolidFaces) {
        int left = lineLeft.minX;
        int right = lineRight.maxX;
        if (left < 0) left = 0;
        if (right > display->windowWidth) right = display->windowWidth;

        switch (renderSettings->lightingMode) {
            case LIGHTING_NONE: {
                for (int x = left; x <= right; x++) {
                    draw_pixel(display, x, lineLeft.y, triangle->color);
                }

                break;
            }

            case LIGHTING_FLAT: {
                uint32_t flatColor = 0xFFFFFFFF;
                float lightFaceAlignment = kcr_vec3_dot(&globalLight->direction, &triangle->normalizedNormal);

                #define MIN_LIGHT 0.1f
                if (lightFaceAlignment < MIN_LIGHT) {
                    lightFaceAlignment = MIN_LIGHT;
                }

                flatColor = modify_color(flatColor, lightFaceAlignment);
                for (int x = left; x <= right; x++) {
                    draw_pixel(display, x, lineLeft.y, flatColor);
                }
            }
        }
    }

    if (renderSettings->showWireframe) {
        uint32_t color = renderSettings->showSolidFaces ? 0xFF000000 : 0xFFFFFFFF;

        for (int x = lineLeft.minX; x <= lineLeft.maxX; x++) {
            draw_pixel(display, x, lineLeft.y, color);
        }

        for (int x = lineRight.minX; x <= lineRight.maxX; x++) {
            draw_pixel(display, x, lineRight.y, color);
        }
    }
}

void perform_render(const struct KCR_Display* display, const struct KCR_RenderSettings *renderSettings,
                    const struct KCR_RenderTriangle* triangle, const struct KCR_GlobalLight *globalLight,
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
    for (int y = startY; y <= endY; y++) {
        struct LineYPoint left = topToBottomPoints[topToBottomIndex];
        struct LineYPoint right = reachedMidPoint
                ? midToBottomPoints[midToBottomIndex]
                : topToMidPoints[topToMidIndex];

        if (left.maxX > right.minX) SWAP(left, right, struct LineYPoint);

        assert(left.y == y);
        assert(right.y == y);

        render_scanline(display, renderSettings, triangle, globalLight, left, right);

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
    struct KCR_Vec4 projectedVector1 = perform_projection(projection, &triangle->v1);
    struct KCR_Vec4 projectedVector2 = perform_projection(projection, &triangle->v2);
    struct KCR_Vec4 projectedVector3 = perform_projection(projection, &triangle->v3);

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
