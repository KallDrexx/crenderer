#ifndef CRENDERER_DISPLAY_H
#define CRENDERER_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

struct KCR_Display_Internal;
struct KCR_Display {
    int windowWidth;
    int windowHeight;
    uint32_t* pixelBuffer;
    struct KCR_Display_Internal* internal;
};

struct KCR_Display* kcr_display_create(void);
void kcr_display_free(struct KCR_Display* display);
void kcr_display_begin_frame(struct KCR_Display* display);
void kcr_display_finish_frame(struct KCR_Display* display);
int kcr_display_get_pixel_index(const struct KCR_Display* display, int x, int y);

#endif //CRENDERER_DISPLAY_H
