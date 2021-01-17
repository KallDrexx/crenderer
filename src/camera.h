#ifndef CRENDERER_CAMERA_H
#define CRENDERER_CAMERA_H

#include "vector.h"

typedef struct {
    Vector3 position;
    Vector3 rotation;
    float field_of_view;
} camera;

#endif //CRENDERER_CAMERA_H
