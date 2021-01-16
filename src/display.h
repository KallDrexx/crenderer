#ifndef CRENDERER_DISPLAY_H
#define CRENDERER_DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL.h>

extern int window_width;
extern int window_height;
extern uint32_t* pixel_buffer;

bool initialize_display(void);
void free_display_resources(void);
void begin_render_cycle(void);
void finish_render_cycle(void);

#endif //CRENDERER_DISPLAY_H
