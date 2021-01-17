#include <SDL2/SDL.h>
#include "display.h"
#include "draw.h"


void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight) {
        pixelBuffer[get_pixel_index(x, y)] = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            draw_pixel(x + i, y + j, color);
        }
    }
}
