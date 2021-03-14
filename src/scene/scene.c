#include <assert.h>
#include "scene.h"
#include "../list.h"
#define MOVE_SPEED 5.0f

size_t meshIndex = 0;

bool kcr_scene_init(struct KCR_Scene* scene, const struct KCR_Display* display) {
    assert(scene != NULL);

    scene->camera = (struct KCR_Camera) {0};
    kcr_camera_init(&scene->camera, display);

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
    instance->position.x = 4;
    instance->position.y = 4;
    instance->position.z = 10;

//    instance = kcr_list_new_item((void**) &scene->instanceList);
//    kcr_mesh_instance_init(instance, &scene->meshList[meshIndex % kcr_list_length(scene->meshList)]);
//    instance->position.x = 5;
//    instance->position.y = 5;
//    instance->position.z = 10;

    return true;
}

void process_camera_inputs(struct KCR_Scene *scene, const struct KCR_InputState *inputState, float timeDelta,
                           const struct KCR_Display *display) {

    struct KCR_Camera_Orientation orientation = kcr_camera_orientation(&scene->camera);

    struct KCR_Vec3 cameraMovement = {0, 0, 0};
    if (inputState->w_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&orientation.forward, MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->s_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&orientation.forward, -MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->a_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&orientation.right, -MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->d_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&orientation.right, MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->page_up_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&orientation.up, MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->page_down_down) {
        struct KCR_Vec3 move = kcr_vec3_mult(&orientation.up, -MOVE_SPEED);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    if (inputState->mouse_wheel_amount != 0) {
        float amount = inputState->mouse_wheel_amount > 0 ? MOVE_SPEED : -MOVE_SPEED;
        struct KCR_Vec3 move = kcr_vec3_mult(&orientation.up, amount);
        move = kcr_vec3_mult(&move, timeDelta);
        cameraMovement = kcr_vec3_add(&cameraMovement, &move);
    }

    scene->camera.position = kcr_vec3_add(&scene->camera.position, &cameraMovement);
//    scene->instanceList[0].rotation.y += 0.5f * timeDelta;

    if (inputState->left_mouse_down) {
        float percentRight = (float) inputState->mouse_drag_x / (float) display->windowWidth;
        float percentUp = (float) inputState->mouse_drag_y / (float) display->windowHeight;
        float angleRight = scene->camera.fieldOfViewRadians * percentRight * 2;
        float angleUp = scene->camera.fieldOfViewRadians * percentUp;

        scene->camera.rotation.y += angleRight;
        scene->camera.rotation.x -= angleUp;
    }
}

void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState, float timeDelta, struct KCR_Display* display) {
    bool meshChanged = false;
    if (inputState->f1_pressed && meshIndex > 0) {
        meshIndex--;
        meshChanged = true;
    }

    if (inputState->f2_pressed) {
        meshIndex++;
        meshChanged = true;
    }

    process_camera_inputs(scene, inputState, timeDelta, display);

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
