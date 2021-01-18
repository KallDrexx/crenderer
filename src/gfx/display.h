#ifndef CRENDERER_DISPLAY_H
#define CRENDERER_DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL.h>

extern int windowWidth;
extern int windowHeight;
extern uint32_t* pixelBuffer;

bool display_init(void);
void display_free(void);
void display_begin_frame(void);
void display_finish_frame(void);
int display_get_pixel_index(int x, int y);

#endif //CRENDERER_DISPLAY_H
