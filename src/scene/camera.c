#include <assert.h>
#include <math.h>
#include "camera.h"
#include "../math/angles.h"

bool kcr_camera_init(struct KCR_Camera* camera, const struct KCR_Display* display) {
    assert(camera != NULL);

    camera->rotation = (struct KCR_Vec3) {0, 0, 0};
    camera->position = (struct KCR_Vec3) {0, 0, 0};
    camera->fieldOfViewRadians = kcr_degrees_to_radians(90);
    camera->zNear = 1.0f;
    camera->zFar = 100.0f;

    kcr_camera_update_frustum(camera, display);

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
         {orientation.right.x, orientation.right.y, orientation.right.z, -rightDot},
         {orientation.up.x, orientation.up.y, orientation.up.z, -upDot},
         {orientation.forward.x, orientation.forward.y, orientation.forward.z, -forwardDot},
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

void kcr_camera_update_frustum(struct KCR_Camera *camera, const struct KCR_Display* display) {
    camera->viewFrustum.planes[PLANE_NEAR] = (struct KCR_Plane) {
        .point = (struct KCR_Vec3) {0, 0, camera->zNear},
        .normal = (struct KCR_Vec3) {0, 0, 1},
    };

    camera->viewFrustum.planes[PLANE_FAR] = (struct KCR_Plane) {
        .point = (struct KCR_Vec3) {0, 0, camera->zFar},
        .normal = (struct KCR_Vec3) {0, 0, -1},
    };

    camera->viewFrustum.planes[PLANE_RIGHT] = (struct KCR_Plane) {
        .point = (struct KCR_Vec3) {0, 0, 0},
        .normal = (struct KCR_Vec3) {
            .x = -cosf(camera->fieldOfViewRadians / 2),
            .y = 0,
            .z = sinf(camera->fieldOfViewRadians / 2),
        },
    };

    camera->viewFrustum.planes[PLANE_LEFT] = (struct KCR_Plane) {
        .point = (struct KCR_Vec3) {0, 0, 0},
        .normal = (struct KCR_Vec3) {
            .x = -camera->viewFrustum.planes[PLANE_RIGHT].normal.x,
            .y = 0,
            .z = camera->viewFrustum.planes[PLANE_RIGHT].normal.z,
        },
    };

    float aspectRatio = (float) display->windowWidth / (float) display->windowHeight;
    float verticalFov = camera->fieldOfViewRadians * aspectRatio;

    camera->viewFrustum.planes[PLANE_TOP] = (struct KCR_Plane) {
        .point = (struct KCR_Vec3) {0, 0, 0},
        .normal = (struct KCR_Vec3) {
            .x = 0,
            .y = -cosf(verticalFov / 2),
            .z = sinf(verticalFov / 2),
        },
    };

    camera->viewFrustum.planes[PLANE_BOTTOM] = (struct KCR_Plane) {
            .point = (struct KCR_Vec3) {0, 0, 0},
            .normal = (struct KCR_Vec3) {
                    .x = 0,
                    .y = -camera->viewFrustum.planes[PLANE_TOP].normal.y,
                    .z = camera->viewFrustum.planes[PLANE_TOP].normal.z,
            },
    };
}
