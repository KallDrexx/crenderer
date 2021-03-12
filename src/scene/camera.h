#ifndef CRENDERER_CAMERA_H
#define CRENDERER_CAMERA_H

#include <stdbool.h>
#include "../math/vector.h"
#include "../math/matrix.h"

/*
 * Represents a camera in the scene
 */
struct KCR_Camera {
    struct KCR_Vec3 position;
    struct KCR_Vec3 up;
    struct KCR_Vec3 right;
    struct KCR_Vec3 forward;
    float fieldOfViewRadians;
    float zNear;
    float zFar;
};

/*
 * Initializes an uninitialized camera
 */
bool kcr_camera_init(struct KCR_Camera* camera);

/*
 * Uninitializes a camera
 */
void kcr_camera_uninit(struct KCR_Camera* camera);

/*
 * Creates a view transform matrix based on the camera's position and orientation
 */
struct KCR_Matrix4 kcr_camera_view_matrix(const struct KCR_Camera* camera);

#endif //CRENDERER_CAMERA_H
