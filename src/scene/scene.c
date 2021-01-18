#include "scene.h"
#include "../math/vector.h"
#include "../gfx/draw.h"
#include "../gfx/display.h"

#define NUM_POINTS (9 * 9 * 9)
Vec3 cubePoints[NUM_POINTS];
Vec3 cameraPosition = {0, 0, -5};

void scene_init(void) {
    int index = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                Vec3 vector = {x,y,z};
                cubePoints[index] = vector;
                index++;
            }
        }
    }
}

void scene_update(InputState *inputState) {

}

Vec2 perform_projection(Vec3 *vector) {
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

    float depth = vector->z - cameraPosition.z;
    Vec2 result = {
            vector->x / depth,
            vector->y / depth
    };

    return result;
}

void scene_render(void) {
    #define PIXELS_PER_UNIT 2000
    #define RECT_SIZE 8

    int centerWidth = windowWidth / 2;
    int centerHeight = windowHeight / 2;
    for (int index = 0; index < NUM_POINTS; index++) {
        Vec2 projectedPoint = perform_projection(&cubePoints[index]);

        float mult = (1 - cubePoints[index].z) / 2.0f;
        uint8_t red = (uint32_t) (((float) 0xFF) * mult);
        uint32_t color = (((uint32_t) red) << 4 * 4) | 0xFF000000;

        draw_rect(
                projectedPoint.x * PIXELS_PER_UNIT + centerWidth,
                projectedPoint.y * PIXELS_PER_UNIT + centerHeight,
                RECT_SIZE,
                RECT_SIZE,
                color);
    }
}
