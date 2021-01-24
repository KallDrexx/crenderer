#ifndef CRENDERER_DRAW_H
#define CRENDERER_DRAW_H

#include <stdint.h>
#include "display.h"

void kcr_draw_pixel(const struct KCR_Display* display, int x, int y, uint32_t color);
void kcr_draw_rect(const struct KCR_Display* display, int x, int y, int width, int height, uint32_t color);
void kcr_draw_line(const struct KCR_Display* display, int x1, int y1, int x2, int y2, uint32_t color);

#endif //CRENDERER_DRAW_H
