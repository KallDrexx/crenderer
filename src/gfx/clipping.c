#include <assert.h>
#include <string.h>
#include "clipping.h"
#include "../list.h"

#define SWAP(FIRST, SECOND, TYPE) do {TYPE SWAP = FIRST; FIRST = SECOND; SECOND = SWAP;} while(0)

// These input and output buffers are used for storing triangleBuffer to be clipped and triangleBuffer that are the result
// of being clipped after a single plane.  These are global to not have to allocate a new set of lists for every
// triangle every frame.
struct KCR_RenderTriangle* inputBuffer = NULL;
struct KCR_RenderTriangle* outputBuffer = NULL;

void kcr_clipping_copy_triangle(const struct KCR_RenderTriangle* input, struct KCR_RenderTriangle* output) {
    memcpy(output, input, sizeof (struct KCR_RenderTriangle));
}

float kcr_clipping_point_plane_alignment(const struct KCR_Plane* plane, const struct KCR_Vec3* point) {
    assert(plane != NULL);

    struct KCR_Vec3 toPoint = kcr_vec3_sub(point, &plane->point);
    toPoint = kcr_vec3_normalize(&toPoint);

    return kcr_vec3_dot(&toPoint, &plane->normal);
}

/*
 * Gets the linear interpolation factor to pinpoint the intersection between two points intersected by a plane
 */
float kcr_clipping_interpolation_factor(float point1PlaneAlignment, float point2PlaneAlignment) {
    return -(point1PlaneAlignment / (point2PlaneAlignment - point1PlaneAlignment));
}

struct KCR_Vec3 kcr_clipping_interpolate_vec3(const struct KCR_Vec3* point1,
        const struct KCR_Vec3* point2,
        float interpolationFactor) {
    struct KCR_Vec3 result = kcr_vec3_sub(point2, point1);
    result = kcr_vec3_mult(&result, interpolationFactor);
    result = kcr_vec3_add(point1, &result);

    return result;
}

struct KCR_Vec2 kcr_clipping_interpolate_vec2(const struct KCR_Vec2* point1,
                                              const struct KCR_Vec2* point2,
                                              float interpolationFactor) {
    struct KCR_Vec2 result = kcr_vec2_sub(point2, point1);
    result = kcr_vec2_mult(&result, interpolationFactor);
    result = kcr_vec2_add(point1, &result);

    return result;
}

struct KCR_Vec3 kcr_clipping_plane_intersection(const struct KCR_Vec3* point1, float point1PlaneAlignment,
                                                const struct KCR_Vec3* point2, float point2PlaneAlignment) {
    float interpolationFactor = kcr_clipping_interpolation_factor(point1PlaneAlignment, point2PlaneAlignment);
    struct KCR_Vec3 result = kcr_vec3_sub(point2, point1);
    result = kcr_vec3_mult(&result, interpolationFactor);
    result = kcr_vec3_add(point1, &result);

    return result;
}

void kcr_clipping_split_double_triangle(const struct KCR_RenderTriangle* input,
        const float alignments[3],
        struct KCR_RenderTriangle** outputList) {
    // This means we have only one point of the triangle outside the clipping plane.  This will result in two
    // triangles being created after clipping.

    int firstIndex = 0;
    while (alignments[firstIndex] < 0) firstIndex++;
    int secondIndex = (firstIndex + 1) % 3;
    int thirdIndex = (firstIndex + 2) % 3;

    struct KCR_RenderTriangle* firstTriangle = kcr_list_new_item((void**) &outputList);
    struct KCR_RenderTriangle* secondTriangle = kcr_list_new_item((void**) &outputList);
    kcr_clipping_copy_triangle(input, firstTriangle);
    kcr_clipping_copy_triangle(input, secondTriangle);

    if (alignments[firstIndex] > 0 && alignments[secondIndex] > 0) {
        // First line is inside the triangle so that line isn't clipped.  So the first triangle becomes
        // v0->v1->ca and the 2nd triangle becomes v0->ca->cb
        float factor12 = kcr_clipping_interpolation_factor(alignments[secondIndex], alignments[thirdIndex]);
        float factor02 = kcr_clipping_interpolation_factor(alignments[firstIndex], alignments[thirdIndex]);

        struct KCR_Vec3 pointCa = kcr_clipping_interpolate_vec3(&input->vertexPositions[secondIndex],
                                                                    &input->vertexPositions[thirdIndex],
                                                                    factor12);

        struct KCR_Vec3 normalCa = kcr_clipping_interpolate_vec3(&input->vertexNormals[secondIndex],
                                                                    &input->vertexNormals[thirdIndex],
                                                                    factor12);

        struct KCR_Vec2 textureCa = kcr_clipping_interpolate_vec2(&input->vertexTextureCoordinates[secondIndex],
                                                                    &input->vertexTextureCoordinates[thirdIndex],
                                                                    factor12);

        struct KCR_Vec3 pointCb = kcr_clipping_interpolate_vec3(&input->vertexPositions[firstIndex],
                                                                  &input->vertexPositions[thirdIndex],
                                                                  factor02);

        struct KCR_Vec3 normalCb = kcr_clipping_interpolate_vec3(&input->vertexNormals[firstIndex],
                                                                   &input->vertexNormals[thirdIndex],
                                                                   factor02);

        struct KCR_Vec2 textureCb = kcr_clipping_interpolate_vec2(&input->vertexTextureCoordinates[firstIndex],
                                                                    &input->vertexTextureCoordinates[thirdIndex],
                                                                    factor02);

        firstTriangle->vertexPositions[thirdIndex] = pointCa;
        firstTriangle->vertexNormals[thirdIndex] = normalCa;
        firstTriangle->vertexTextureCoordinates[thirdIndex] = textureCa;

        secondTriangle->vertexPositions[secondIndex] = pointCa;
        secondTriangle->vertexNormals[secondIndex] = normalCa;
        secondTriangle->vertexTextureCoordinates[secondIndex] = textureCa;
        secondTriangle->vertexPositions[thirdIndex] = pointCb;
        secondTriangle->vertexNormals[thirdIndex] = normalCb;
        secondTriangle->vertexTextureCoordinates[thirdIndex] = textureCb;
    }
    else {
        // First line is clipped.  So the first triangle becomes v0->ca->cb and the second v0->cb->v2
        float factor01 = kcr_clipping_interpolation_factor(alignments[firstIndex], alignments[secondIndex]);
        float factor12 = kcr_clipping_interpolation_factor(alignments[secondIndex], alignments[thirdIndex]);

        struct KCR_Vec3 pointCa = kcr_clipping_interpolate_vec3(&input->vertexPositions[firstIndex],
                                                                  &input->vertexPositions[secondIndex],
                                                                  factor01);

        struct KCR_Vec3 normalCa = kcr_clipping_interpolate_vec3(&input->vertexNormals[firstIndex],
                                                                   &input->vertexNormals[secondIndex],
                                                                   factor01);

        struct KCR_Vec2 textureCa = kcr_clipping_interpolate_vec2(&input->vertexTextureCoordinates[firstIndex],
                                                                    &input->vertexTextureCoordinates[secondIndex],
                                                                    factor01);

        struct KCR_Vec3 pointCb = kcr_clipping_interpolate_vec3(&input->vertexPositions[secondIndex],
                                                                  &input->vertexPositions[thirdIndex],
                                                                  factor12);

        struct KCR_Vec3 normalCb = kcr_clipping_interpolate_vec3(&input->vertexNormals[secondIndex],
                                                                   &input->vertexNormals[thirdIndex],
                                                                   factor12);

        struct KCR_Vec2 textureCb = kcr_clipping_interpolate_vec2(&input->vertexTextureCoordinates[secondIndex],
                                                                    &input->vertexTextureCoordinates[thirdIndex],
                                                                    factor12);

        firstTriangle->vertexPositions[secondIndex] = pointCa;
        firstTriangle->vertexNormals[secondIndex] = normalCa;
        firstTriangle->vertexTextureCoordinates[secondIndex] = textureCa;
        firstTriangle->vertexPositions[thirdIndex] = pointCb;
        firstTriangle->vertexNormals[thirdIndex] = normalCb;
        firstTriangle->vertexTextureCoordinates[thirdIndex] = textureCb;

        secondTriangle->vertexPositions[secondIndex] = pointCb;
        secondTriangle->vertexNormals[secondIndex] = normalCb;
        secondTriangle->vertexTextureCoordinates[secondIndex] = textureCb;
    }
}

void kcr_clipping_split_single_triangle(const struct KCR_RenderTriangle* input,
        const float alignments[3],
        struct KCR_RenderTriangle** outputList) {
    // This means we have two points of the triangle outside of the clipping plane.  This will result in a single
    // triangle being created after clipping.  So we need to start at the vertex that's inside the triangle and walk
    // the lines.
    int insideIndex = 0;
    while (alignments[insideIndex] < 0) insideIndex++;

    struct KCR_RenderTriangle* newTriangle = kcr_list_new_item((void**) &outputList);
    kcr_clipping_copy_triangle(input, newTriangle);

    for (int count = 0; count < 2; count++) {
        int outsideIndex = (insideIndex + 1 + count) % 3;
        float factor = kcr_clipping_interpolation_factor(alignments[insideIndex], alignments[outsideIndex]);

        newTriangle->vertexPositions[outsideIndex] = kcr_clipping_interpolate_vec3(&input->vertexPositions[insideIndex],
                                                                                   &input->vertexPositions[outsideIndex],
                                                                                   factor);

        newTriangle->vertexNormals[outsideIndex] = kcr_clipping_interpolate_vec3(&input->vertexNormals[insideIndex],
                                                                                 &input->vertexNormals[outsideIndex],
                                                                                 factor);

        newTriangle->vertexTextureCoordinates[outsideIndex] = kcr_clipping_interpolate_vec2(&input->vertexTextureCoordinates[insideIndex],
                                                                                            &input->vertexTextureCoordinates[outsideIndex],
                                                                                            factor);
    }
}

void kcr_clipping_clip_triangle(const struct KCR_RenderTriangle* triangle,
                                const struct KCR_ViewFrustum* frustum,
                                struct KCR_RenderTriangle** renderList) {
    if (inputBuffer == NULL) inputBuffer = kcr_list_create(sizeof (struct KCR_RenderTriangle));
    if (outputBuffer == NULL) outputBuffer = kcr_list_create(sizeof (struct KCR_RenderTriangle));

    kcr_list_clear(inputBuffer);
    kcr_list_clear(outputBuffer);

    struct KCR_RenderTriangle* firstOutput = kcr_list_new_item((void **) &outputBuffer);
    kcr_clipping_copy_triangle(triangle, firstOutput);

    for (int pi = PLANE_NEAR; pi <= PLANE_BOTTOM; pi++) {
//    {int pi = PLANE_RIGHT;
        const struct KCR_Plane* plane = &frustum->planes[pi];

        // Use the output from clipping the last plane as the input for clipping this plane
        SWAP(inputBuffer, outputBuffer, struct KCR_RenderTriangle*);
        kcr_list_clear(outputBuffer);

        uint32_t inputCount = kcr_list_length(inputBuffer);
        for (uint32_t ti = 0; ti < inputCount; ti++) {
            struct KCR_RenderTriangle* current = &inputBuffer[ti];
            float alignments[3] = {
                    kcr_clipping_point_plane_alignment(plane, &current->vertexPositions[0]),
                    kcr_clipping_point_plane_alignment(plane, &current->vertexPositions[1]),
                    kcr_clipping_point_plane_alignment(plane, &current->vertexPositions[2]),
            };

            int numNegative = 0;
            if (alignments[0] < 0) numNegative++;
            if (alignments[1] < 0) numNegative++;
            if (alignments[2] < 0) numNegative++;

            switch (numNegative) {
                case 0: {
                    // All points are on or inside the plane, so include the whole triangle
                    struct KCR_RenderTriangle* outputTriangle = kcr_list_new_item((void**) &outputBuffer);
                    kcr_clipping_copy_triangle(current, outputTriangle);

                    continue;
                }

                case 1:
                    kcr_clipping_split_double_triangle(current, alignments, outputBuffer);
                    break;

                case 2:
                    // Two points are outside the triangle, which would clip this into 2 smaller triangles
                    kcr_clipping_split_single_triangle(current, alignments, outputBuffer);
                    break;

                case 3:
                    // All points are outside the plane, so ignore this triangle
                    break;

                default:
                    exit(-500); // shouldn't happen so hard crash if it does
            }
        }
    }

    // Clipped against all planes, add all items from the output buffer into the render list
    uint32_t outputCount = kcr_list_length(outputBuffer);
    for (uint32_t i = 0; i < outputCount; i++) {
        struct KCR_RenderTriangle* fromOutput = &outputBuffer[i];
        struct KCR_RenderTriangle* fromRenderList = kcr_list_new_item((void**) renderList);
        memcpy(fromRenderList, fromOutput, sizeof (struct KCR_RenderTriangle));
    }
}
