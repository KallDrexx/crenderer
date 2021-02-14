#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "renderer.h"
#include "triangle_renderer.h"
#include "../list.h"

#define SWAP(FIRST, SECOND, TYPE) do {TYPE SWAP = FIRST; FIRST = SECOND; SECOND = SWAP;} while(0)

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
    if (inputState->one_pressed) {
        renderer->renderMode.showVertices = !renderer->renderMode.showVertices;
    }

    if (inputState->two_pressed) {
        renderer->renderMode.showWireframe = !renderer->renderMode.showWireframe;
    }

    if (inputState->three_pressed) {
        renderer->renderMode.showSolidFaces = !renderer->renderMode.showSolidFaces;
    }

    if (inputState->four_pressed) {
        switch(renderer->renderMode.lightingMode) {
            case LIGHTING_NONE:
                renderer->renderMode.lightingMode = LIGHTING_FLAT;
                break;

            case LIGHTING_FLAT:
                renderer->renderMode.lightingMode = LIGHTING_NONE;
                break;
        }
    }

    if (!renderer->renderMode.showWireframe && !renderer->renderMode.showSolidFaces) {
        renderer->renderMode.showWireframe = true;
    }
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

            render_triangle(renderer->display, &renderer->renderMode, triangle, &scene->globalLight, &projectionMatrix);
        }
    }
}
