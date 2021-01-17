#include <math.h>
#include "vector.h"

float vector3_length(Vector3* vector) {
    float total = vector->x * vector->x +
            vector->y * vector->y +
            vector->z * vector->z;

    return sqrtf(total);
}

float vector2_length(Vector2* vector) {
    float total = vector->x * vector->x +
            vector->y * vector->y;

    return sqrtf(total);
}

Vector3 vector3_add(Vector3 *first, Vector3 *second) {
    Vector3 result = {
            first->x + second->x,
            first->y + second->y,
            first->z + second->z,
    };

    return result;
}

Vector2 vector2_add(Vector2 *first, Vector2 *second) {
    Vector2 result = {
            first->x + second->x,
            first->y + second->y,
    };

    return result;
}

Vector2 project_to_2d(Vector3 *vector) {
    Vector2 result = { vector->x, vector->y };
    return result;
}

Vector2 vector3_project(Vector3 *vector, Vector3 *eye) {
    // We need to calculate where the x and y coordinates are when using perspective projection.  Therefore, if we
    // have a vector for the location of the eye we want to figure out where on some projection plane in between the
    // eye and the point the point would look at.
    //
    // To do this we need to envision looking at the top top (x,z plane) and side (y,z plane) between the point and
    // the eye.  This creates two right hand triangles, one from the eye to the spot on the projection plane where
    // the point will be projected to, and one from the eye to the point's position in 3d space.  These triangles will
    // have the same angles and therefore are considered scaled from one another.
    //
    // Since both triangles are different scales of the same, we need to figure out what ratio the larger triangle
    // (the one to the real point) is compared to the smaller one.  Once we have the ratio we can apply that ratio
    // to the x and y values to figure out where on each axis that point is projected to on the projection plane.
    //
    // Thus if we have:
    //   AB = depth between the eye to the projection plane
    //   AD = depth between the eye to the point
    //   DE = length of the point on the desired axis from the eye
    //   BC = Length of the projected point on the desired axis from the eye (what we want to figure out)
    //
    // We end up with the formula (AB / AD = BC / DE) which is then simplified to BC = DE * (AB / AD).  AB and AD
    // are always the same for both axis we want to project on.
    //
    // If we decide that the eye is always one unit behind the projection plane, then we can simplify this to be
    // BC = DE / AD.  This is known as the "perspective divide" equation.


    float eyeToVecDepth = vector->z - eye->z;

    Vector2 result = {
            vector->x / eyeToVecDepth,
            vector->y / eyeToVecDepth,
    };

    return result;
}

