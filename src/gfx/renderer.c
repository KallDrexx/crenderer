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

    triangle->vertexPositions[0] = kcr_mat4_vec3_mult(transform, &mesh->vertexList[face->v1.positionIndex]);
    triangle->vertexPositions[1] = kcr_mat4_vec3_mult(transform, &mesh->vertexList[face->v2.positionIndex]);
    triangle->vertexPositions[2] = kcr_mat4_vec3_mult(transform, &mesh->vertexList[face->v3.positionIndex]);

    // Don't translate the normal, just rotate and scale.
    struct KCR_Matrix4 normalTransform = *transform;
    normalTransform.m[0][3] = 0;
    normalTransform.m[1][3] = 0;
    normalTransform.m[2][3] = 0;
    triangle->vertexNormals[0] = kcr_mat4_vec3_mult(&normalTransform, &mesh->normalList[face->v1.normalIndex]);
    triangle->vertexNormals[1] = kcr_mat4_vec3_mult(&normalTransform, &mesh->normalList[face->v2.normalIndex]);
    triangle->vertexNormals[2] = kcr_mat4_vec3_mult(&normalTransform, &mesh->normalList[face->v3.normalIndex]);

    // normalize the normal vectors for alignment determination
    triangle->vertexNormals[0] = kcr_vec3_normalize(&triangle->vertexNormals[0]);
    triangle->vertexNormals[1] = kcr_vec3_normalize(&triangle->vertexNormals[1]);
    triangle->vertexNormals[2] = kcr_vec3_normalize(&triangle->vertexNormals[2]);

    triangle->vertexTextureCoordinates[0] = mesh->textureCoordsList[face->v1.textureIndex];
    triangle->vertexTextureCoordinates[1] = mesh->textureCoordsList[face->v2.textureIndex];
    triangle->vertexTextureCoordinates[2] = mesh->textureCoordsList[face->v3.textureIndex];

    triangle->color = face->color;
    triangle->averageDepth = (triangle->vertexPositions[0].z + triangle->vertexPositions[1].z + triangle->vertexPositions[2].z) / 3;

    const struct KCR_Vec3 v1 = kcr_vec3_sub(&triangle->vertexPositions[1], &triangle->vertexPositions[0]);
    const struct KCR_Vec3 v2 = kcr_vec3_sub(&triangle->vertexPositions[2], &triangle->vertexPositions[0]);
    triangle->normalizedTriangleNormal = kcr_vec3_cross(&v1, &v2);
    triangle->normalizedTriangleNormal = kcr_vec3_normalize(&triangle->normalizedTriangleNormal);

    triangle->texture = mesh->texture;
}

void update_render_mode(struct KCR_Renderer *renderer, const struct KCR_InputState *inputState) {
    if (inputState->one_pressed) {
        renderer->renderMode.showWireframe = !renderer->renderMode.showWireframe;
        if (!renderer->renderMode.showWireframe && renderer->renderMode.triangleFillMode == FILL_NONE) {
            renderer->renderMode.showWireframe = true;
        }
    }

    if (inputState->two_pressed) {
        switch(renderer->renderMode.triangleFillMode) {
            case FILL_NONE:
                renderer->renderMode.triangleFillMode = FILL_MESH_TRI_COLORS;
                break;

            case FILL_MESH_TRI_COLORS:
                renderer->renderMode.triangleFillMode = FILL_WHITE;
                break;

            case FILL_WHITE:
                renderer->renderMode.triangleFillMode = FILL_TEXTURED;
                break;

            case FILL_TEXTURED:
                renderer->renderMode.triangleFillMode = renderer->renderMode.showWireframe
                                                        ? FILL_NONE
                                                        : FILL_MESH_TRI_COLORS;
        }
    }

    if (inputState->three_pressed) {
        switch(renderer->renderMode.lightingMode) {
            case LIGHTING_NONE:
                renderer->renderMode.lightingMode = LIGHTING_FLAT;
                break;

            case LIGHTING_FLAT:
                renderer->renderMode.lightingMode = LIGHTING_SMOOTH;
                break;

            case LIGHTING_SMOOTH:
                renderer->renderMode.lightingMode = LIGHTING_NONE;
                break;
        }
    }

    if (inputState->c_pressed) {
        renderer->renderMode.enableBackFaceCulling = !renderer->renderMode.enableBackFaceCulling;
    }
}

bool kcr_renderer_init(struct KCR_Renderer *renderer, const struct KCR_Display *display) {
    assert(renderer != NULL);
    assert(display != NULL);

    renderer->display = display;
    renderer->triangles = kcr_list_create(sizeof(struct KCR_RenderTriangle));
    renderer->renderMode.showWireframe = false;
    renderer->renderMode.triangleFillMode = FILL_MESH_TRI_COLORS;
    renderer->renderMode.enableBackFaceCulling = true;
    renderer->zBuffer = malloc(sizeof(float) * display->windowWidth * display->windowHeight);

    return true;
}

void kcr_renderer_uninit(struct KCR_Renderer *renderer) {
    if (renderer != NULL) {
        renderer->display = NULL;
        free(renderer->zBuffer);
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
    assert(renderer->zBuffer != NULL);

    update_render_mode(renderer, inputState);
    for (int i = 0; i < renderer->display->windowHeight * renderer->display->windowWidth; i++) {
        renderer->zBuffer[i] = 0.0f;
    }

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

    struct KCR_Matrix4 viewTransform = kcr_camera_view_matrix(&scene->camera);

    size_t triangleIndex = 0;
    for (size_t i = 0; i < kcr_list_length(scene->instanceList); i++) {
        struct KCR_MeshInstance* instance = &scene->instanceList[i];
        struct KCR_Matrix4 scale = kcr_mat4_scale(instance->scale.x, instance->scale.y, instance->scale.z);
        struct KCR_Matrix4 translation = kcr_mat4_translate(instance->position.x, instance->position.y, instance->position.z);
        struct KCR_Matrix4 rotationX = kcr_mat4_rotation_x(instance->rotation.x);
        struct KCR_Matrix4 rotationY = kcr_mat4_rotation_y(instance->rotation.y);
        struct KCR_Matrix4 rotationZ = kcr_mat4_rotation_z(instance->rotation.z);

        struct KCR_Matrix4 transform = kcr_mat4_identity();
        transform = kcr_mat4_mult(&scale, &transform);
        transform = kcr_mat4_mult(&rotationZ, &transform);
        transform = kcr_mat4_mult(&rotationY, &transform);
        transform = kcr_mat4_mult(&rotationX, &transform);
        transform = kcr_mat4_mult(&translation, &transform);
        transform = kcr_mat4_mult(&viewTransform, &transform);
        
        for (size_t f = 0; f < kcr_list_length(instance->mesh->faceList); f++) {
            struct KCR_RenderTriangle* renderTriangle = &renderer->triangles[triangleIndex];
            const struct KCR_Face* face = &instance->mesh->faceList[f];

            transform_face(renderTriangle, face, instance->mesh, &transform);
            triangleIndex++;
        }
    }

    for (triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++) {
//        if (triangleIndex != 8) continue;;
        struct KCR_RenderTriangle* triangle = &renderer->triangles[triangleIndex];

//        const float alignment = kcr_vec3_dot(&triangle->normalizedTriangleNormal, &cameraOrientation.forward);
        render_triangle(renderer->display, &renderer->renderMode, triangle, &scene->globalLight, &projectionMatrix, renderer->zBuffer);
    }
}
