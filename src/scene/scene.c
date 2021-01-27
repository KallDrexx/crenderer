#include <malloc.h>
#include "scene.h"

struct KCR_Scene* kcr_scene_create(void) {
    struct KCR_Scene* scene = malloc(sizeof(struct KCR_Scene));

    scene->cameraPosition.x = 0;
    scene->cameraPosition.y = 0;
    scene->cameraPosition.z = -8;

    scene->mesh = kcr_mesh_from_obj_file("assets/cube.obj");

    return scene;
}

void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState, float timeDelta) {
    #define KEYBOARD_ROTATION_SPEED 0.5f
    #define MOUSE_ROTATION_SPEED 0.005f

    if (inputState->up_pressed) scene->mesh->rotation.x += KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->down_pressed) scene->mesh->rotation.x -= KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->left_pressed) scene->mesh->rotation.y += KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->right_pressed) scene->mesh->rotation.y -= KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->home_pressed) scene->mesh->rotation.z -= KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->end_pressed) scene->mesh->rotation.z += KEYBOARD_ROTATION_SPEED * timeDelta;

    if (inputState->left_mouse_down) {
        scene->mesh->rotation.y -= MOUSE_ROTATION_SPEED * inputState->mouse_drag_x;
        scene->mesh->rotation.x -= MOUSE_ROTATION_SPEED * inputState->mouse_drag_y;
    }
}

void kcr_scene_free(struct KCR_Scene *scene) {
    if (scene != NULL) {
        kcr_mesh_free(scene->mesh);
    }

    free(scene);
}
