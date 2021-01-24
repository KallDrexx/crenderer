#include <malloc.h>
#include "scene.h"

struct KCR_Scene* kcr_scene_create(void) {
    struct KCR_Scene* scene = malloc(sizeof(struct KCR_Scene));

    scene->cameraPosition.x = 0;
    scene->cameraPosition.y = 0;
    scene->cameraPosition.z = -8;

    scene->cubeRotation.x = 0;
    scene->cubeRotation.y = 0;
    scene->cubeRotation.z = 0;

    scene->cube = kcr_mesh_create_cube();

    return scene;
}

void kcr_scene_update(struct KCR_Scene* scene, const struct KCR_InputState* inputState, float timeDelta) {
    #define KEYBOARD_ROTATION_SPEED 0.5f
    #define MOUSE_ROTATION_SPEED 0.005f

    if (inputState->up_pressed) scene->cubeRotation.x += KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->down_pressed) scene->cubeRotation.x -= KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->left_pressed) scene->cubeRotation.y += KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->right_pressed) scene->cubeRotation.y -= KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->home_pressed) scene->cubeRotation.z -= KEYBOARD_ROTATION_SPEED * timeDelta;
    if (inputState->end_pressed) scene->cubeRotation.z += KEYBOARD_ROTATION_SPEED * timeDelta;

    if (inputState->left_mouse_down) {
        scene->cubeRotation.y -= MOUSE_ROTATION_SPEED * inputState->mouse_drag_x;
        scene->cubeRotation.x -= MOUSE_ROTATION_SPEED * inputState->mouse_drag_y;
    }
}

void kcr_scene_free(struct KCR_Scene *scene) {
    if (scene != NULL) {
        kcr_mesh_free(scene->cube);
    }

    free(scene);
}
