#ifndef CRENDERER_ANGLES_H
#define CRENDERER_ANGLES_H

#include <math.h>

static inline float kcr_degrees_to_radians(float degrees) {
    return M_PI / 180 * degrees;
}

static inline float kcr_radians_to_degrees(float radians) {
    return (float) (180.0 / M_PI * radians);
}

#endif //CRENDERER_ANGLES_H
