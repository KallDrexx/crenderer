#include <assert.h>
#include "scene.h"
#include "../list.h"

size_t meshIndex = 0;

bool kcr_scene_init(struct KCR_Scene* scene) {
    assert(scene != NULL);

    scene->camera = (struct KCR_Camera) {0};
    kcr_camera_init(&scene->camera);
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

    if (kcr_mesh_load_asset(f22, "drone") == false) {
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

void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState, float timeDelta, struct KCR_Display* display) {
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

    struct KCR_Vec3 cameraMovement = {0, 0, 0};
    if (inputState->w_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&scene->camera.forward, MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->s_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&scene->camera.forward, -MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->a_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&scene->camera.right, -MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->d_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&scene->camera.right, MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    scene->camera.position = kcr_vec3_add(&scene->camera.position, &cameraMovement);

    if (inputState->left_mouse_down) {
        float percentRight = (float) inputState->mouse_drag_x / (float) display->windowWidth;
        float angleRight = scene->camera.fieldOfViewRadians * percentRight * 2;
        scene->camera.forward = kcr_vec3_rotate_y(&scene->camera.forward, angleRight);
        scene->camera.right = kcr_vec3_rotate_y(&scene->camera.right, angleRight);
    }

    for (size_t i = 0; i < kcr_list_length(scene->instanceList); i++) {
        struct KCR_MeshInstance *instance = &scene->instanceList[i];

        if (meshChanged) {
            size_t index = meshIndex % kcr_list_length(scene->meshList);
            struct KCR_Mesh* mesh = &scene->meshList[index];
            instance->mesh = mesh;
            instance->scale = (struct KCR_Vec3) {1.0f, 1.0f, 1.0f};
        }
    }
}

void kcr_scene_uninit(struct KCR_Scene *scene) {
    if (scene != NULL) {
        kcr_camera_uninit(&scene->camera);

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
