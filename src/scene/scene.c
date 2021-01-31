#include <assert.h>
#include "scene.h"
#include "../list.h"

size_t meshIndex = 1;

bool kcr_scene_init(struct KCR_Scene* scene) {
    assert(scene != NULL);

    scene->cameraPosition.x = 0;
    scene->cameraPosition.y = 0;
    scene->cameraPosition.z = 0;

    scene->meshList = kcr_list_create(sizeof(struct KCR_Mesh));
    assert(scene->meshList != NULL);

    struct KCR_Mesh* teapot = (struct KCR_Mesh*) kcr_list_new_item((void**) &scene->meshList);
    struct KCR_Mesh* cube = kcr_list_new_item((void**) &scene->meshList);
    struct KCR_Mesh* f22 = kcr_list_new_item((void**) &scene->meshList);
    assert(kcr_list_length(scene->meshList) == 3);

    if (kcr_mesh_from_obj_file(teapot, "assets/teapot.obj") == false) {
        return false;
    }

    if (kcr_mesh_from_obj_file(cube, "assets/cube.obj") == false) {
        return false;
    }

    if (kcr_mesh_from_obj_file(f22, "assets/f22.obj") == false) {
        return false;
    }

    scene->instanceList = kcr_list_create(sizeof(struct KCR_MeshInstance));
    assert(scene->instanceList != NULL);

    struct KCR_MeshInstance* instance = kcr_list_new_item((void**) &scene->instanceList);
    kcr_mesh_instance_init(instance, &scene->meshList[meshIndex % kcr_list_length(scene->meshList)]);

    instance->position.x = 0;
    instance->position.y = 0;
    instance->position.z = -100;

    return true;
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

void kcr_scene_uninit(struct KCR_Scene *scene) {
    if (scene != NULL) {
        if (scene->instanceList != NULL) {
            for (size_t x = 0; x < kcr_list_length(scene->instanceList); x++) {
                kcr_mesh_instance_uninit(&scene->instanceList[x]);
            }

            for (size_t x = 0; x < kcr_list_length(scene->meshList); x++) {
                kcr_mesh_uninit(&scene->meshList[x]);
            }

            kcr_list_free(scene->instanceList);
            kcr_list_free(scene->meshList);

            scene->instanceList = NULL;
            scene->meshList = NULL;
        }
    }
}
