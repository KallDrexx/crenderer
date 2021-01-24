#include <math.h>
#include "display.h"
#include "draw.h"

void kcr_draw_pixel(const struct KCR_Display* display, int x, int y, uint32_t color) {
    if (x >= 0 && x < display->windowWidth && y >= 0 && y < display->windowHeight) {
        int index = kcr_display_get_pixel_index(display, x, y);
        display->pixelBuffer[index] = color;
    }
}

void kcr_draw_rect(const struct KCR_Display* display, int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            kcr_draw_pixel(display, x + i, y + j, color);
        }
    }
}

void kcr_draw_line(const struct KCR_Display *display, int x1, int y1, int x2, int y2, uint32_t color) {
    const int changeInY = y2 - y1;
    const int changeInX = x2 - x1;

    // Use separate variables to prevent incorrect slope
    const int minX = x1 < x2 ? x1 : x2;
    const int maxX = x1 < x2 ? x2 : x1;
    const int minY = y1 < y2 ? y1 : y2;
    const int maxY = y1 < y2 ? y2 : y1;

    if (changeInX == 0) {
        // straight line down
        for (int y = minY; y <= maxY; y++) {
            kcr_draw_pixel(display, x1, y, color);
        }
    } else if (changeInY == 0) {
        // straight line across
        for (int x = minX; x <= maxX; x++) {
            kcr_draw_pixel(display, x, y1, color);
        }
    } else {
        // sloped line
        float slope = (float) changeInY / (float) changeInX;
        const float yAxisIntercept = (float) y1 - slope * (float) x1;

        // We need to know if we need to walk the x or y axis to make sure the line is fully filled in, otherwise we
        // will end up with a dotted line due to incrementing too fast.  Make sure to absolute value the changes
        // since we got the changes prior to normalizing the direction.
        if (abs(changeInX) < abs(changeInY)) {
            for (int y = minY; y <= maxY; y++) {
                int x = (int) (((float)y - yAxisIntercept) / slope);
                kcr_draw_pixel(display, x, y, color);
            }
        } else {
            for (int x = minX; x <= maxX; x++) {
                int y = (int) (slope * (float) x + yAxisIntercept);
                kcr_draw_pixel(display, x, y, color);
            }
        }
    }
}
