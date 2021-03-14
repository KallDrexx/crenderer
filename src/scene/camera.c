#include <assert.h>
#include <stdlib.h>
#include "camera.h"
#include "../math/angles.h"

bool kcr_camera_init(struct KCR_Camera* camera) {
    assert(camera != NULL);

    camera->rotation = (struct KCR_Vec3) {0, 0, 0};
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

    struct KCR_Matrix4 rotationX = kcr_mat4_rotation_x(camera->rotation.x);
    struct KCR_Matrix4 rotationY = kcr_mat4_rotation_y(camera->rotation.y);
    struct KCR_Matrix4 rotationZ = kcr_mat4_rotation_z(camera->rotation.z);

    struct KCR_Matrix4 rotation = kcr_mat4_mult(&rotationY, &rotationZ);
    rotation = kcr_mat4_mult(&rotationX, &rotation);

    result.forward = kcr_mat4_vec3_mult(&rotation, &result.forward);
    result.up = kcr_mat4_vec3_mult(&rotation, &result.up);
    result.right = kcr_mat4_vec3_mult(&rotation, &result.right);

    return result;
}