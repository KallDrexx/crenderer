#include <assert.h>
#include "scene.h"
#include "../list.h"
#include "../math/angles.h"

size_t meshIndex = 0;

bool kcr_scene_init(struct KCR_Scene* scene) {
    assert(scene != NULL);

    scene->camera = (struct KCR_Camera) {0};
    scene->camera.zNear = 1.0f;
    scene->camera.zFar = 100.0f;
    scene->camera.fieldOfViewRadians = kcr_degrees_to_radians(60);
    scene->globalLight.direction.x = 1;
    scene->globalLight.direction.y = -1;
    scene->globalLight.direction.z = -1;
    scene->globalLight.direction = kcr_vec3_normalize(&scene->globalLight.direction);

    scene->meshList = kcr_list_create(sizeof(struct KCR_Mesh));
    assert(scene->meshList != NULL);

    struct KCR_Mesh* teapot = (struct KCR_Mesh*) kcr_list_new_item((void**) &scene->meshList);
    struct KCR_Mesh* cube = kcr_list_new_item((void**) &scene->meshList);
    struct KCR_Mesh* f22 = kcr_list_new_item((void**) &scene->meshList);
    assert(kcr_list_length(scene->meshList) == 3);

    if (kcr_mesh_load_asset(teapot, "cube") == false) {
        return false;
    }

    if (kcr_mesh_load_asset(cube, "f22") == false) {
        return false;
    }

    if (kcr_mesh_load_asset(f22, "crab") == false) {
        return false;
    }

    scene->instanceList = kcr_list_create(sizeof(struct KCR_MeshInstance));
    assert(scene->instanceList != NULL);

    struct KCR_MeshInstance* instance = kcr_list_new_item((void**) &scene->instanceList);
    kcr_mesh_instance_init(instance, &scene->meshList[meshIndex % kcr_list_length(scene->meshList)]);
    instance->position.x = 0;
    instance->position.y = 0;
    instance->position.z = -10;

    return true;
}

void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState, float timeDelta) {
    #define KEYBOARD_ROTATION_SPEED 1.0f
    #define MOUSE_ROTATION_SPEED 0.01f
    #define MOVE_SPEED 5.0f

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

        if (inputState->up_down) instance->position.y += MOVE_SPEED * timeDelta;
        if (inputState->down_down) instance->position.y -= MOVE_SPEED * timeDelta;
        if (inputState->left_down) instance->position.x -= MOVE_SPEED * timeDelta;
        if (inputState->right_down) instance->position.x += MOVE_SPEED * timeDelta;
        if (inputState->home_down) instance->rotation.z -= KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->end_down) instance->rotation.z += KEYBOARD_ROTATION_SPEED * timeDelta;
        if (inputState->minus_down) instance->position.z -= MOVE_SPEED * timeDelta;
        if (inputState->equals_down) instance->position.z += MOVE_SPEED * timeDelta;

        if (inputState->left_mouse_down) {
            instance->rotation.y += MOUSE_ROTATION_SPEED * inputState->mouse_drag_x;
            instance->rotation.x += MOUSE_ROTATION_SPEED * inputState->mouse_drag_y;
        }

        if (meshChanged) {
            size_t index = meshIndex % kcr_list_length(scene->meshList);
            struct KCR_Mesh* mesh = &scene->meshList[index];
            instance->mesh = mesh;
            instance->scale = index == 0
                    ? (struct KCR_Vec3) {0.25f, 0.25f, 0.25f}
                    : (struct KCR_Vec3) {1.0f, 1.0f, 1.0f};
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
