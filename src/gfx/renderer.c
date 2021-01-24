#include "renderer.h"
#include "draw.h"

struct TransformedFace {
    struct KCR_Vec3 v1;
    struct KCR_Vec3 v2;
    struct KCR_Vec3 v3;
};

struct KCR_Vec2 perform_projection(const struct KCR_Scene* scene, const struct KCR_Vec3 *vector) {
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

    float depth = vector->z - scene->cameraPosition.z;
    struct KCR_Vec2 result = {
            vector->x / depth,
            vector->y / depth
    };

    return result;
}

struct TransformedFace transformFace(const struct KCR_Face* face, const struct KCR_Mesh* mesh, const struct KCR_Vec3* rotation) {
    struct TransformedFace transformedFace = {
            mesh->vertices[face->vertexIndex1],
            mesh->vertices[face->vertexIndex2],
            mesh->vertices[face->vertexIndex3],
    };

    transformedFace.v1 = kcr_vec3_rotate_x(&transformedFace.v1, rotation->x);
    transformedFace.v1 = kcr_vec3_rotate_y(&transformedFace.v1, rotation->y);
    transformedFace.v1 = kcr_vec3_rotate_z(&transformedFace.v1, rotation->z);

    transformedFace.v2 = kcr_vec3_rotate_x(&transformedFace.v2, rotation->x);
    transformedFace.v2 = kcr_vec3_rotate_y(&transformedFace.v2, rotation->y);
    transformedFace.v2 = kcr_vec3_rotate_z(&transformedFace.v2, rotation->z);

    transformedFace.v3 = kcr_vec3_rotate_x(&transformedFace.v3, rotation->x);
    transformedFace.v3 = kcr_vec3_rotate_y(&transformedFace.v3, rotation->y);
    transformedFace.v3 = kcr_vec3_rotate_z(&transformedFace.v3, rotation->z);

    return transformedFace;
}

void render_face(const struct KCR_Display* display, const struct KCR_Scene* scene, const struct TransformedFace* face) {
    #define PIXELS_PER_UNIT 2000
    #define RECT_SIZE 8

    struct KCR_Vec2 projectedPoints[] = {
            perform_projection(scene, &face->v1),
            perform_projection(scene, &face->v2),
            perform_projection(scene, &face->v3),
    };

    int centerWidth = display->windowWidth / 2;
    int centerHeight = display->windowHeight / 2;
    for (int i = 0; i < 3; i++) {
        struct KCR_Vec2* point = &projectedPoints[i];

        // Adjust for the screen positioning
        point->x = point->x * PIXELS_PER_UNIT + (float) centerWidth;
        point->y = point->y * PIXELS_PER_UNIT + (float) centerHeight;

        kcr_draw_rect(display, (int) point->x, (int) point->y, RECT_SIZE, RECT_SIZE, 0xFFFFFFFF);
    }
}

void kcr_render(const struct KCR_Display *display, const struct KCR_Scene *scene) {
    for (int idx = 0; idx < scene->cube->faceCount; idx++) {
        const struct KCR_Face* face = &scene->cube->faces[idx];
        const struct TransformedFace transformedFace = transformFace(face, scene->cube, &scene->cubeRotation);
        render_face(display, scene, &transformedFace);
    }
}
