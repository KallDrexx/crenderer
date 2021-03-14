#ifndef CRENDERER_CAMERA_H
#define CRENDERER_CAMERA_H

#include <stdbool.h>
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../gfx/clipping.h"
#include "../gfx/display.h"

/*
 * Represents a camera in the scene
 */
struct KCR_Camera {
    struct KCR_Vec3 position;
    struct KCR_Vec3 rotation;
    float fieldOfViewRadians;
    float zNear;
    float zFar;

    /*
     * The camera's view frustum in camera space
     */
    struct KCR_ViewFrustum viewFrustum;
};

/*
 * Represents the vectors that represent the current state of the camera's forward, right, and up vectors
 */
struct KCR_Camera_Orientation {
    struct KCR_Vec3 forward;
    struct KCR_Vec3 right;
    struct KCR_Vec3 up;
};

/*
 * Initializes an uninitialized camera
 */
bool kcr_camera_init(struct KCR_Camera* camera, const struct KCR_Display* display);

/*
 * Uninitializes a camera
 */
void kcr_camera_uninit(struct KCR_Camera* camera);

/*
 * Creates a view transform matrix based on the camera's position and orientation
 */
struct KCR_Matrix4 kcr_camera_view_matrix(const struct KCR_Camera* camera);

/*
 * Gets the camera's current orientation
 */
struct KCR_Camera_Orientation kcr_camera_orientation(const struct KCR_Camera* camera);

/*
 * Updates the camera's view frustum.  Needed anytime the field of view, znear, or zfar changes changes
 */
void kcr_camera_update_frustum(struct KCR_Camera* camera, const struct KCR_Display* display);

#endif //CRENDERER_CAMERA_H
