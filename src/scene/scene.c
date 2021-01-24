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

void kcr_scene_update(struct KCR_Scene* scene, float timeDelta) {
    scene->cubeRotation.x += 0.5f * timeDelta;
    scene->cubeRotation.y += 0.5f * timeDelta;
    scene->cubeRotation.z += 0.5f * timeDelta;
}

void kcr_scene_free(struct KCR_Scene *scene) {
    if (scene != NULL) {
        kcr_mesh_free(scene->cube);
    }

    free(scene);
}
