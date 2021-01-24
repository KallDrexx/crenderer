#include <math.h>
#include "angles.h"

float kcr_degrees_to_radians(float degrees) {
    return M_PI / 180 * degrees;
}

float kcr_radians_to_degrees(float radians) {
    return 180 / M_PI * radians;
}
