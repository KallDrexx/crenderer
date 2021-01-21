#include <malloc.h>
#include <stdbool.h>
#include "scene.h"
#include "../math/vector.h"
#include "../gfx/draw.h"

#define NUM_POINTS (9 * 9 * 9)
struct KCR_Scene_Internal {
    struct KCR_Vec3 cubePoints[NUM_POINTS];
    struct KCR_Vec3 cameraPosition;
    struct KCR_Vec3 cubeRotation;
};

struct KCR_Scene* kcr_scene_init(void) {
    struct KCR_Scene* scene = malloc(sizeof(struct KCR_Scene));
    scene->internal = malloc(sizeof(struct KCR_Scene_Internal));

    scene->internal->cameraPosition.x = 0;
    scene->internal->cameraPosition.y = 0;
    scene->internal->cameraPosition.z = -8;

    int index = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                struct KCR_Vec3 vector = {x, y, z};
                scene->internal->cubePoints[index] = vector;
                index++;
            }
        }
    }

    return scene;
}

void kcr_scene_update(const struct KCR_Scene* scene) {
    scene->internal->cubeRotation.x += 0.01f;
    scene->internal->cubeRotation.y += 0.01f;
    scene->internal->cubeRotation.z += 0.01f;
}

struct KCR_Vec2 perform_projection(struct KCR_Scene* scene, struct KCR_Vec3 *vector) {
    // basic perspective projection.  Assumes camera is facing down the z axis, no FOV considerations, and
    // projection plane is one unit in front of the camera.  Based on the idea that the triangle between the camera,
    // projection plane, and where the vector is projected to on the plane (what we need to figure out) is a similar
    // triangle to the camera to the center of the vector in 3d space, to the real position on the current axis of the
    // vector (x or y depending on which axis of the projection plane we are projecting onto at that time).
    //
    // This allows us to say for certain that since the ratio of the depth between the camera to the projection plane
    // and the depth between the camera and the vector in 3d space is the same as the ratio between the camera to the
    // vector on the calculated axis and the camera to the vector on the projected plane.  This gives us the equation
    // a / b = c / d, where
    //   * a = opposite length of smaller triangle (what we want to calculate)
    //   * b = opposite length of the larger triangle
    //   * c = adjacent length of the smaller triangle
    //   * d = adjacent length of the larger triangle
    //
    // Since we are assuming the camera is 1 unit in front of the camera, this allows us to calculate the answer
    // by simplifying the equation to a = b / d.

    float depth = vector->z - scene->internal->cameraPosition.z;
    struct KCR_Vec2 result = {
            vector->x / depth,
            vector->y / depth
    };

    return result;
}

void kcr_scene_render(struct KCR_Scene* scene, struct KCR_Display* display) {
    #define PIXELS_PER_UNIT 2000
    #define RECT_SIZE 8

    int centerWidth = display->windowWidth / 2;
    int centerHeight = display->windowHeight / 2;
    for (int index = 0; index < NUM_POINTS; index++) {
        struct KCR_Vec3 rotatedPointX = vec3_rotate_x(&scene->internal->cubePoints[index], scene->internal->cubeRotation.x);
        struct KCR_Vec3 rotatedPointY = vec3_rotate_y(&rotatedPointX, scene->internal->cubeRotation.y);
        struct KCR_Vec3 rotatedPointFinal = vec3_rotate_z(&rotatedPointY, scene->internal->cubeRotation.z);

        struct KCR_Vec2 projectedPoint = perform_projection(scene, &rotatedPointFinal);

        draw_rect(
                display,
                projectedPoint.x * PIXELS_PER_UNIT + centerWidth,
                projectedPoint.y * PIXELS_PER_UNIT + centerHeight,
                RECT_SIZE,
                RECT_SIZE,
                0xFFFF0000);
    }
}

void kcr_scene_free(struct KCR_Scene *scene) {
    if (scene != NULL) {
        free(scene->internal);
    }

    free(scene);
}
