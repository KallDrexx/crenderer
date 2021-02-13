#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "renderer.h"
#include "triangle_renderer.h"
#include "../list.h"

#define SWAP(FIRST, SECOND, TYPE) do {TYPE SWAP = FIRST; FIRST = SECOND; SECOND = SWAP;} while(0)

/*
 * Gets the value of a color when multiplied by a value.
 */
uint32_t modify_color(uint32_t original, float multiplier) {
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



void transform_face(struct KCR_RenderTriangle* triangle,
                    const struct KCR_Face* face,
                    const struct KCR_Mesh* mesh,
                    const struct KCR_Matrix4* transform) {

    triangle->v1 = mesh->vertexList[face->vertexIndex1];
    triangle->v2 = mesh->vertexList[face->vertexIndex2];
    triangle->v3 = mesh->vertexList[face->vertexIndex3];
    triangle->color = face->color;

    triangle->v1 = kcr_mat4_vec3_mult(transform, &triangle->v1);
    triangle->v2 = kcr_mat4_vec3_mult(transform, &triangle->v2);
    triangle->v3 = kcr_mat4_vec3_mult(transform, &triangle->v3);

    triangle->averageDepth = (triangle->v1.z + triangle->v2.z + triangle->v3.z) / 3;
}

void update_render_mode(struct KCR_Renderer *renderer, const struct KCR_InputState *inputState) {
    if (!inputState->one_pressed &&
        !inputState->two_pressed &&
        !inputState->three_pressed &&
        !inputState->four_pressed &&
        !inputState->five_pressed &&
        !inputState->c_pressed) {
        return;
    }

    if (inputState->one_pressed) {
        renderer->renderMode.showSolidFaces = false;
        renderer->renderMode.showVertices = false;
        renderer->renderMode.enableLighting = false;
        renderer->renderMode.showWireframe = true;
    }

    if (inputState->two_pressed) {
        renderer->renderMode.showSolidFaces = false;
        renderer->renderMode.enableLighting = false;
        renderer->renderMode.showVertices = true;
        renderer->renderMode.showWireframe = true;
    }

    if (inputState->three_pressed) {
        renderer->renderMode.enableLighting = false;
        renderer->renderMode.showVertices = false;
        renderer->renderMode.showWireframe = false;
        renderer->renderMode.showSolidFaces = true;
    }

    if (inputState->four_pressed) {
        renderer->renderMode.enableLighting = false;
        renderer->renderMode.showVertices = false;
        renderer->renderMode.showSolidFaces = true;
        renderer->renderMode.showWireframe = true;
    }

    if (inputState->five_pressed) {
        renderer->renderMode.showVertices = false;
        renderer->renderMode.showWireframe = false;
        renderer->renderMode.showSolidFaces = true;
        renderer->renderMode.enableLighting = true;
    }

    if (inputState->c_pressed) {
        renderer->renderMode.enableBackFaceCulling = !renderer->renderMode.enableBackFaceCulling;
    }
}

void render_face(const struct KCR_Display* display,
                 const struct KCR_Scene* scene,
                 struct KCR_RenderTriangle* triangle,
                 const struct KCR_RenderSettings* renderMode,
                 const struct KCR_Matrix4* projection) {
    render_triangle(display, renderMode, triangle, &scene->globalLight, projection);
}

int sort_triangle_function(const void* a, const void* b) {
    float depthA = ((struct KCR_RenderTriangle*) a)->averageDepth;
    float depthB = ((struct KCR_RenderTriangle*) b)->averageDepth;

    if (depthB < depthA) return 1;
    if (depthB > depthA) return -1;
    return 0;
}

bool kcr_renderer_init(struct KCR_Renderer *renderer, const struct KCR_Display *display) {
    assert(renderer != NULL);
    assert(display != NULL);

    renderer->display = display;
    renderer->triangles = kcr_list_create(sizeof(struct KCR_RenderTriangle));
    renderer->renderMode.showWireframe = false;
    renderer->renderMode.showVertices = false;
    renderer->renderMode.showSolidFaces = true;
    renderer->renderMode.enableBackFaceCulling = true;

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
    struct KCR_Matrix4 projectionMatrix = kcr_mat4_perspective(
            scene->camera.fieldOfViewRadians,
            (float) renderer->display->windowHeight / (float) renderer->display->windowWidth,
            scene->camera.zNear,
            scene->camera.zFar
        );
    
    size_t triangleCount = 0;
    for (size_t i = 0; i < kcr_list_length(scene->instanceList); i++) {
        triangleCount += kcr_list_length(scene->instanceList[i].mesh->faceList);
    }

    size_t listLength = kcr_list_length(renderer->triangles);
    size_t newTriangles = triangleCount - kcr_list_length(renderer->triangles);
    if (listLength < triangleCount && newTriangles > 0) {
        kcr_list_new_items((void**) &renderer->triangles, newTriangles);
    }
    
    size_t triangleIndex = 0;
    for (size_t i = 0; i < kcr_list_length(scene->instanceList); i++) {
        struct KCR_MeshInstance* instance = &scene->instanceList[i];
        struct KCR_Matrix4 translation = kcr_mat4_translate(instance->position.x, instance->position.y, instance->position.z);
        struct KCR_Matrix4 rotationX = kcr_mat4_rotation_x(instance->rotation.x);
        struct KCR_Matrix4 rotationY = kcr_mat4_rotation_y(instance->rotation.y);
        struct KCR_Matrix4 rotationZ = kcr_mat4_rotation_z(instance->rotation.z);

        struct KCR_Matrix4 transform = kcr_mat4_identity();
        // scale goes here
        transform = kcr_mat4_mult(&rotationZ, &transform);
        transform = kcr_mat4_mult(&rotationY, &transform);
        transform = kcr_mat4_mult(&rotationX, &transform);
        transform = kcr_mat4_mult(&translation, &transform);
        
        for (size_t f = 0; f < kcr_list_length(instance->mesh->faceList); f++) {
            struct KCR_RenderTriangle* renderTriangle = &renderer->triangles[triangleIndex];
            const struct KCR_Face* face = &instance->mesh->faceList[f];

            transform_face(renderTriangle, face, instance->mesh, &transform);
            triangleIndex++;
        }
    }

    qsort(renderer->triangles, triangleCount, sizeof(struct KCR_RenderTriangle), sort_triangle_function);

    for (triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++) {
        struct KCR_RenderTriangle* triangle = &renderer->triangles[triangleIndex];

        const struct KCR_Vec3 v1 = kcr_vec3_sub(&triangle->v2, &triangle->v1);
        const struct KCR_Vec3 v2 = kcr_vec3_sub(&triangle->v3, &triangle->v1);
        triangle->normalizedNormal = kcr_vec3_cross(&v1, &v2);
        triangle->normalizedNormal = kcr_vec3_normalize(&triangle->normalizedNormal);

        const struct KCR_Vec3 vertexToCameraVector = kcr_vec3_sub(&triangle->v1, &scene->camera.position);
        const float vertexCameraAlignment = kcr_vec3_dot(&triangle->normalizedNormal, &vertexToCameraVector);

        if (!renderer->renderMode.enableBackFaceCulling || vertexCameraAlignment > 0) {
            // Since the normal is pointing in generally the same direction as the vector of the face to the camera
            // then the face is facing towards the camera, and we need to render it

            if (renderer->renderMode.enableLighting) {
                float lightFaceAlignment = kcr_vec3_dot(&triangle->normalizedNormal, &scene->globalLight.direction);

                #define MIN_LIGHT 0.1f
                if (lightFaceAlignment < MIN_LIGHT) {
                    lightFaceAlignment = MIN_LIGHT;
                }

                triangle->color = modify_color(0xFFFFFFFF, lightFaceAlignment);
            }

            render_face(renderer->display, scene, triangle, &renderer->renderMode, &projectionMatrix);
        }
    }
}
