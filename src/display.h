#ifndef CRENDERER_DISPLAY_H
#define CRENDERER_DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL.h>

extern int windowWidth;
extern int windowHeight;
extern uint32_t* pixelBuffer;

bool initialize_display(void);
void free_display_resources(void);
void begin_render_cycle(void);
void finish_render_cycle(void);
int get_pixel_index(int x, int y);

#endif //CRENDERER_DISPLAY_H
