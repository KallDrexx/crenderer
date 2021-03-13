#include <assert.h>
#include <stdlib.h>
#include "camera.h"
#include "../math/angles.h"

bool kcr_camera_init(struct KCR_Camera* camera) {
    assert(camera != NULL);

    camera->yaw = 0;
    camera->pitch = 0;
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

    struct KCR_Camera_Orientation orientation = kcr_camera_orientation(camera);

    float rightDot = kcr_vec3_dot(&orientation.right, &camera->position);
    float upDot = kcr_vec3_dot(&orientation.up, &camera->position);
    float forwardDot = kcr_vec3_dot(&orientation.forward, &camera->position);

    struct KCR_Matrix4 viewMatrix = {{
         {orientation.right.x, orientation.right.y, orientation.right.z, rightDot},
         {orientation.up.x, orientation.up.y, orientation.up.z, upDot},
         {orientation.forward.x, orientation.forward.y, orientation.forward.z, forwardDot},
         {0, 0, 0, 1},
    }};

    return viewMatrix;
}

struct KCR_Camera_Orientation kcr_camera_orientation(const struct KCR_Camera *camera) {
    struct KCR_Camera_Orientation result = {
            (struct KCR_Vec3) {0, 0, 1},
            (struct KCR_Vec3) {1, 0, 0},
            (struct KCR_Vec3) {0, 1, 0},
    };

    result.forward = kcr_vec3_rotate_y(&result.forward, camera->yaw);
    result.forward = kcr_vec3_rotate_x(&result.forward, camera->pitch);

    result.up = kcr_vec3_rotate_y(&result.up, camera->yaw);
    result.up = kcr_vec3_rotate_x(&result.up, camera->pitch);

    result.right = kcr_vec3_rotate_y(&result.right, camera->yaw);
    result.right = kcr_vec3_rotate_x(&result.right, camera->pitch);

    return result;
}