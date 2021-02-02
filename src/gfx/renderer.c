#include <math.h>
#include <assert.h>
#include "renderer.h"
#include "../list.h"

#define SWAP(FIRST, SECOND, TYPE) do {TYPE SWAP = FIRST; FIRST = SECOND; SECOND = SWAP;} while(0)

struct TransformedFace {
    struct KCR_Vec3 v1;
    struct KCR_Vec3 v2;
    struct KCR_Vec3 v3;
};

static inline void draw_pixel(const struct KCR_Display* display, int x, int y, uint32_t color) {
    if (x >= 0 && x < display->windowWidth && y >= 0 && y < display->windowHeight) {
        int index = kcr_display_get_pixel_index(display, x, y);
        display->pixelBuffer[index] = color;
    }
}

void draw_rect(const struct KCR_Display* display, int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            draw_pixel(display, x + i, y + j, color);
        }
    }
}

void draw_line(const struct KCR_Display* display, int x1, int y1, int x2, int y2, uint32_t color) {
    const int changeInY = y2 - y1;
    const int changeInX = x2 - x1;
    const int numberOfPoints = abs(changeInY) > abs(changeInX) ? abs(changeInY) : abs(changeInX);
    const float xIncrement = (float) changeInX / (float) numberOfPoints;
    const float yIncrement = (float) changeInY / (float) numberOfPoints;

    float currentX = (float) x1;
    float currentY = (float) y1;
    for (int i = 0; i <= numberOfPoints; i++) {
        draw_pixel(display, (int) roundf(currentX), (int) roundf(currentY), color);
        currentX += xIncrement;
        currentY += yIncrement;
    }
}

void draw_filled_triangle(const struct KCR_Display* display, const struct KCR_Vec2* vertices[3], uint32_t color) {
    // Get top to bottom indexes using screen coordinates (topleft = 0,0, y increases as going down
    int topIndex = 0,
        midIndex = 1,
        bottomIndex = 2;

    if (vertices[topIndex]->y > vertices[midIndex]->y) SWAP(topIndex, midIndex, int);
    if (vertices[midIndex]->y > vertices[bottomIndex]->y) SWAP(midIndex, bottomIndex, int);
    if (vertices[topIndex]->y > vertices[midIndex]->y) SWAP(topIndex, midIndex, int);

    // Consider the triangle as 2 triangles split at the middle point, so they have consistent Y start and end values.
    // Flat bottom triangle
    if (vertices[topIndex]->y < vertices[midIndex]->y) { // Not a horizontal line from top to mid
        float v01InverseSlope = (float)((int)vertices[midIndex]->x - (int)vertices[topIndex]->x) / (float)((int)vertices[midIndex]->y - (int)vertices[topIndex]->y);
        float v02InverseSlope = (float)((int)vertices[bottomIndex]->x - (int)vertices[topIndex]->x) / (float)((int)vertices[bottomIndex]->y - (int)vertices[topIndex]->y);
        float v01CurX = vertices[topIndex]->x;
        float v02CurX = vertices[topIndex]->x;
        for (int y = (int) vertices[topIndex]->y; y <= (int) vertices[midIndex]->y; y++) {
            int leftX = (int) v01CurX;
            int rightX = (int) v02CurX;
            if (y > 0 && y < display->windowHeight) {
                if (leftX > rightX) SWAP(leftX, rightX, int);
                if (leftX < 0) leftX = 0;
                if (rightX > display->windowWidth) rightX = display->windowWidth;
                for (int x = leftX; x <= rightX; x++) {
                    draw_pixel(display, x, y, color);
                }
            }

            v01CurX += v01InverseSlope;
            v02CurX += v02InverseSlope;
        }
    }

    // flat top triangle
    if (vertices[bottomIndex]->y > vertices[midIndex]->y) { // Not a horizontal line from bottom to mid
        float v21InverseSlope = (float)((int)vertices[midIndex]->x - (int)vertices[bottomIndex]->x) / (float)((int)vertices[midIndex]->y - (int)vertices[bottomIndex]->y);
        float v20InverseSlope = (float)((int)vertices[topIndex]->x - (int)vertices[bottomIndex]->x) / (float)((int)vertices[topIndex]->y - (int)vertices[bottomIndex]->y);
        float v21CurX = vertices[bottomIndex]->x;
        float v20CurX = vertices[bottomIndex]->x;
        for (int y = (int) vertices[bottomIndex]->y; y > (int) vertices[midIndex]->y; y--) {
            int leftX = (int) v21CurX;
            int rightX = (int) v20CurX;
            if (y > 0 && y < display->windowHeight) {
                if (leftX > rightX) SWAP(leftX, rightX, int);
                if (leftX < 0) leftX = 0;
                if (rightX > display->windowWidth) rightX = display->windowWidth;
                for (int x = leftX; x <= rightX; x++) {
                    draw_pixel(display, x, y, color);
                }
            }

            v21CurX -= v21InverseSlope;
            v20CurX -= v20InverseSlope;
        }
    }
}

struct KCR_Vec2 perform_projection(const struct KCR_Scene* scene, const struct KCR_Vec3 *vector) {
    // basic perspective projection.  Assumes camera is facing down the z axis, no FOV considerations, and
    // projection plane is one unit in front of the camera.  Based on the idea that the triangle between the camera,
    // projection plane, and where the vector is projected to on the plane (what we need to figure out) is a similar
    // triangle to the camera to the center of the vector in 3d space, to the real position on the current axis of the
    // vector (x or y depending on which axis of the projection plane we are projecting onto at that time).
    //
    // This allows us to say for certain that since the ratio of the depth between the camera to the projection plane
    // and the depth between the camera and the vector in 3d space is the same as the ratio between the camera to the
    // vector on the calculated axis and the camera to the vector on the projected plane.  This gives us the equation
    // a / b = c / d, where
    //   * a = opposite length of smaller triangle (what we want to calculate)
    //   * b = opposite length of the larger triangle
    //   * c = adjacent length of the smaller triangle
    //   * d = adjacent length of the larger triangle
    //
    // Since we are assuming the camera is 1 unit in front of the camera, this allows us to calculate the answer
    // by simplifying the equation to a = b / d.

    float depth = vector->z - scene->cameraPosition.z;
    struct KCR_Vec2 result = {
            vector->x / depth,
            vector->y / depth
    };

    return result;
}

void adjust_to_screen_space(const struct KCR_Display* display, struct KCR_Vec2* point) {
    #define PIXELS_PER_UNIT 2000

    int centerWidth = display->windowWidth / 2;
    int centerHeight = display->windowHeight / 2;

    // Negate y value to accommodate the texture being positive Y going down
    point->x = point->x * PIXELS_PER_UNIT + (float) centerWidth;
    point->y = -point->y * PIXELS_PER_UNIT + (float) centerHeight;
}

struct TransformedFace transformFace(const struct KCR_Face* face,
        const struct KCR_Mesh* mesh,
        const struct KCR_Vec3* rotation,
        const struct KCR_Vec3* position) {

    struct TransformedFace transformedFace = {
            mesh->vertexList[face->vertexIndex1],
            mesh->vertexList[face->vertexIndex2],
            mesh->vertexList[face->vertexIndex3],
    };

    transformedFace.v1 = kcr_vec3_rotate_x(&transformedFace.v1, rotation->x);
    transformedFace.v1 = kcr_vec3_rotate_y(&transformedFace.v1, rotation->y);
    transformedFace.v1 = kcr_vec3_rotate_z(&transformedFace.v1, rotation->z);
    transformedFace.v1.x += position->x;
    transformedFace.v1.y += position->y;
    transformedFace.v1.z += position->z;

    transformedFace.v2 = kcr_vec3_rotate_x(&transformedFace.v2, rotation->x);
    transformedFace.v2 = kcr_vec3_rotate_y(&transformedFace.v2, rotation->y);
    transformedFace.v2 = kcr_vec3_rotate_z(&transformedFace.v2, rotation->z);
    transformedFace.v2.x += position->x;
    transformedFace.v2.y += position->y;
    transformedFace.v2.z += position->z;

    transformedFace.v3 = kcr_vec3_rotate_x(&transformedFace.v3, rotation->x);
    transformedFace.v3 = kcr_vec3_rotate_y(&transformedFace.v3, rotation->y);
    transformedFace.v3 = kcr_vec3_rotate_z(&transformedFace.v3, rotation->z);
    transformedFace.v3.x += position->x;
    transformedFace.v3.y += position->y;
    transformedFace.v3.z += position->z;

    return transformedFace;
}

void update_render_mode(struct KCR_Renderer *renderer, const struct KCR_InputState *inputState) {
    if (!inputState->one_pressed &&
        !inputState->two_pressed &&
        !inputState->three_pressed &&
        !inputState->four_pressed) {
        return;
    }

    renderer->renderMode.showSolidFaces = false;
    renderer->renderMode.showVertices = false;
    renderer->renderMode.showWireframe = false;

    if (inputState->one_pressed) {
        renderer->renderMode.showWireframe = true;
    }

    else if (inputState->two_pressed) {
        renderer->renderMode.showVertices = true;
        renderer->renderMode.showWireframe = true;
    }

    else if (inputState->three_pressed) {
        renderer->renderMode.showSolidFaces = true;
    }

    else if (inputState->four_pressed) {
        renderer->renderMode.showSolidFaces = true;
        renderer->renderMode.showWireframe = true;
    }
}

void render_face(const struct KCR_Display* display,
                 const struct KCR_Scene* scene,
                 const struct TransformedFace* face,
                 const struct KCR_RenderMode* renderMode) {

    struct KCR_Vec2 projectedPoint1 = perform_projection(scene, &face->v1);
    struct KCR_Vec2 projectedPoint2 = perform_projection(scene, &face->v2);
    struct KCR_Vec2 projectedPoint3 = perform_projection(scene, &face->v3);

    adjust_to_screen_space(display, &projectedPoint1);
    adjust_to_screen_space(display, &projectedPoint2);
    adjust_to_screen_space(display, &projectedPoint3);

    if (renderMode->showSolidFaces) {
        const struct KCR_Vec2* points[] = {&projectedPoint1, &projectedPoint2, &projectedPoint3};
        draw_filled_triangle(display, points, 0xFFFFFFFF);
    }

    if (renderMode->showWireframe) {
        uint32_t color = renderMode->showSolidFaces ? 0xFF000000 : 0xFFFFFFFF;
        draw_line(display, (int) projectedPoint1.x, (int) projectedPoint1.y, (int) projectedPoint2.x, (int) projectedPoint2.y, color);
        draw_line(display, (int) projectedPoint2.x, (int) projectedPoint2.y, (int) projectedPoint3.x, (int) projectedPoint3.y, color);
        draw_line(display, (int) projectedPoint3.x, (int) projectedPoint3.y, (int) projectedPoint1.x, (int) projectedPoint1.y, color);
    }

    if (renderMode->showVertices) {
        #define RECT_SIZE 4.0f
        #define RECT_HALF_SIZE RECT_SIZE / 2
        draw_rect(display,
                  (int) (projectedPoint1.x - RECT_HALF_SIZE),
                  (int) (projectedPoint1.y - RECT_HALF_SIZE),
                  RECT_SIZE,
                  RECT_SIZE,
                  0xFFFF0000);

        draw_rect(display,
                  (int) (projectedPoint2.x - RECT_HALF_SIZE),
                  (int) (projectedPoint2.y - RECT_HALF_SIZE),
                  RECT_SIZE,
                  RECT_SIZE,
                  0xFFFF0000);

        draw_rect(display,
                  (int) (projectedPoint3.x - RECT_HALF_SIZE),
                  (int) (projectedPoint3.y - RECT_HALF_SIZE),
                  RECT_SIZE,
                  RECT_SIZE,
                  0xFFFF0000);
    }
}

bool kcr_renderer_init(struct KCR_Renderer *renderer, const struct KCR_Display *display) {
    assert(renderer != NULL);
    assert(display != NULL);

    renderer->display = display;
    renderer->renderMode.showWireframe = false;
    renderer->renderMode.showVertices = false;
    renderer->renderMode.showSolidFaces = true;

    return true;
}

void kcr_renderer_uninit(struct KCR_Renderer *renderer) {
    if (renderer != NULL) {
        renderer->display = NULL;
    }
}

void kcr_renderer_render(struct KCR_Renderer *renderer,
                         const struct KCR_Scene *scene,
                         const struct KCR_InputState *inputState) {
    assert(renderer != NULL);
    assert(renderer->display != NULL);
    assert(scene != NULL);
    assert(scene->instanceList != NULL);
    assert(scene->meshList != NULL);
    assert(inputState != NULL);

    update_render_mode(renderer, inputState);

    for (size_t i = 0; i < kcr_list_length(scene->instanceList); i++) {
        struct KCR_MeshInstance* instance = &scene->instanceList[i];

        for (size_t f = 0; f < kcr_list_length(instance->mesh->faceList); f++) {
//            if (f != 0) continue;
            const struct KCR_Face* face = &instance->mesh->faceList[f];
            const struct TransformedFace transformedFace = transformFace(face,
                                                                         instance->mesh,
                                                                         &instance->rotation,
                                                                         &instance->position);

            const struct KCR_Vec3 v1 = kcr_vec3_sub(&transformedFace.v2, &transformedFace.v1);
            const struct KCR_Vec3 v2 = kcr_vec3_sub(&transformedFace.v3, &transformedFace.v1);
            const struct KCR_Vec3 normal = kcr_vec3_cross(&v1, &v2);
            const struct KCR_Vec3 vertexToCameraVector = kcr_vec3_sub(&transformedFace.v1, &scene->cameraPosition);
            const float alignment = kcr_vec3_dot(&normal, &vertexToCameraVector);

            if (alignment > 0) {
                // Since the normal is pointing in generally the same direction as the vector of the face to the camera
                // then the face is facing towards the camera, and we need to cull.
                render_face(renderer->display, scene, &transformedFace, &renderer->renderMode);
            }
        }
    }
}
