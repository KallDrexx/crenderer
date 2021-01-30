#include <malloc.h>
#include <assert.h>
#include "scene.h"
#include "../list.h"

size_t meshIndex = 0;

struct KCR_Scene* kcr_scene_create(void) {
    struct KCR_Scene* scene = malloc(sizeof(struct KCR_Scene));

    scene->cameraPosition.x = 0;
    scene->cameraPosition.y = 0;
    scene->cameraPosition.z = 0;

    struct KCR_Mesh* teapot = kcr_mesh_from_obj_file("assets/teapot.obj");
    struct KCR_Mesh* cube = kcr_mesh_from_obj_file("assets/cube.obj");
    struct KCR_Mesh* f22 = kcr_mesh_from_obj_file("assets/f22.obj");

    assert(teapot != NULL);
    assert(cube != NULL);
    assert(f22 != NULL);

    scene->meshList = kcr_list_create(sizeof(struct KCR_Mesh));
    assert(scene->meshList != NULL);

    kcr_list_append((void**) &scene->meshList, teapot);
    kcr_list_append((void**) &scene->meshList, cube);
    kcr_list_append((void**) &scene->meshList, f22);
    assert(kcr_list_length(scene->meshList) == 3);

    scene->instanceList = kcr_list_create(sizeof(struct KCR_MeshInstance));
    assert(scene->instanceList != NULL);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            struct KCR_MeshInstance* instance = kcr_mesh_instance_create(teapot);
            instance->position.x = (float) x * 20;
            instance->position.y = (float) y * 20;
            instance->position.z = -100;

            kcr_list_append((void**) &scene->instanceList, instance);
        }
    }

    return scene;
}

void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState, float timeDelta) {
    #define KEYBOARD_ROTATION_SPEED 0.5f
    #define MOUSE_ROTATION_SPEED 0.005f
    #define ZOOM_SPEED 50.0f

    bool meshChanged = false;
    if (inputState->f1_pressed && meshIndex > 0) {
        meshIndex--;
        meshChanged = true;
    }

    if (inputState->f2_pressed) {
        meshIndex++;
        meshChanged = true;
    }

    for (size_t i = 0; i < kcr_list_length(scene->instanceList); i++) {
        struct KCR_MeshInstance *instance = &scene->instanceList[i];

        if (inputState->up_down) instance->rotation.x += KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->down_down) instance->rotation.x -= KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->left_down) instance->rotation.y += KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->right_down) instance->rotation.y -= KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->home_down) instance->rotation.z -= KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->end_down) instance->rotation.z += KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->minus_down) instance->position.z -= ZOOM_SPEED * timeDelta;
        if (inputState->equals_down) instance->position.z += ZOOM_SPEED * timeDelta;

        if (inputState->left_mouse_down) {
            instance->rotation.y -= MOUSE_ROTATION_SPEED * inputState->mouse_drag_x;
            instance->rotation.x -= MOUSE_ROTATION_SPEED * inputState->mouse_drag_y;
        }

        if (meshChanged) {
            size_t index = meshIndex % kcr_list_length(scene->meshList);
            struct KCR_Mesh* mesh = &scene->meshList[index];
            instance->mesh = mesh;
        }
    }
}

void kcr_scene_free(struct KCR_Scene *scene) {
    if (scene != NULL) {
        if (scene->instanceList != NULL) {
            for (size_t x = 0; x < kcr_list_length(scene->instanceList); x++) {
                struct KCR_MeshInstance* instance = &scene->instanceList[x];
                kcr_mesh_instance_free(instance);
            }

            for (size_t x = 0; x < kcr_list_length(scene->meshList); x++) {
                struct KCR_Mesh* mesh = &scene->meshList[x];
                kcr_mesh_free(mesh);
            }

            kcr_list_free(scene->instanceList);
            kcr_list_free(scene->meshList);
        }
    }

    free(scene);
}
