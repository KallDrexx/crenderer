#ifndef CRENDERER_CLIPPING_H
#define CRENDERER_CLIPPING_H

#include <stdbool.h>
#include "../math/vector.h"
#include "triangle_renderer.h"

/*
 * Represents an infinite plane in 3d space
 */
struct KCR_Plane {
    struct KCR_Vec3 point;
    struct KCR_Vec3 normal;
};

/*
 * Represents the indexes of the different planes in a frustum.
 */
enum KCR_Frustum_Plane {
    PLANE_NEAR = 0,
    PLANE_FAR = 1,
    PLANE_RIGHT = 2,
    PLANE_LEFT = 3,
    PLANE_TOP = 4,
    PLANE_BOTTOM = 5,
};

/*
 * Represents the six planes that make up the view frustum
 */
struct KCR_ViewFrustum {
    struct KCR_Plane planes[PLANE_BOTTOM + 1];
};

/*
 * Clips a triangle based on the view frustum being passed in.  All resulting triangleBuffer to be rendered
 * are appended to the passed in render list.  These calculations assume that the triangle being clipped
 * is already transformed to the same coordinate space as the view frustum, but not projected to screen space.
 */
void kcr_clipping_clip_triangle(const struct KCR_RenderTriangle* triangle,
                                const struct KCR_ViewFrustum* frustum,
                                struct KCR_RenderTriangle** renderList);

#endif //CRENDERER_CLIPPING_H
