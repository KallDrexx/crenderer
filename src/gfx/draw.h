#ifndef CRENDERER_DRAW_H
#define CRENDERER_DRAW_H

#include <stdint.h>
#include "display.h"

void draw_pixel(struct KCR_Display* display, int x, int y, uint32_t color);
void draw_rect(struct KCR_Display* display, int x, int y, int width, int height, uint32_t color);

#endif //CRENDERER_DRAW_H
