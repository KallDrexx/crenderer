#include <assert.h>
#include <stdlib.h>
#include "camera.h"
#include "../math/angles.h"

bool kcr_camera_init(struct KCR_Camera* camera) {
    assert(camera != NULL);

    camera->up = (struct KCR_Vec3) {0, 1, 0};
    camera->right = (struct KCR_Vec3) {1, 0, 0};
    camera->forward = (struct KCR_Vec3) {0, 0, 1};
    camera->position = (struct KCR_Vec3) {0, 0, 0};
    camera->fieldOfViewRadians = kcr_degrees_to_radians(60);
    camera->zNear = 1.0f;
    camera->zNear = 100.0f;

    return true;
}

void kcr_camera_uninit(struct KCR_Camera* camera) {
    assert(camera != NULL);

    // nothing to free at the moment
}

struct KCR_Matrix4 kcr_camera_view_matrix(const struct KCR_Camera* camera) {
    assert(camera != NULL);

    float rightDot = kcr_vec3_dot(&camera->right, &camera->position);
    float upDot = kcr_vec3_dot(&camera->up, &camera->position);
    float forwardDot = kcr_vec3_dot(&camera->forward, &camera->position);

    struct KCR_Matrix4 viewMatrix = {{
         {camera->right.x, camera->right.y, camera->right.z, rightDot},
         {camera->up.x, camera->up.y, camera->up.z, upDot},
         {camera->forward.x, camera->forward.y, camera->forward.z, forwardDot},
         {0, 0, 0, 1},
    }};

    return viewMatrix;
}