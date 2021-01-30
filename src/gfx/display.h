#ifndef CRENDERER_DISPLAY_H
#define CRENDERER_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

struct KCR_Display_Internal;

/*
 * Structure that manages the application window and pushing pixels to the screen.
 */
struct KCR_Display {
    int windowWidth;
    int windowHeight;
    uint32_t* pixelBuffer;
    struct KCR_Display_Internal* internal;
};

/*
 * Initializes the display to allow rendering of pixels to the screen.  Only an not-yet initialized
 * display can be initialized.
 */
bool kcr_display_init(struct KCR_Display* display);

/*
 * Un-initializes the display and deallocates any memory it's in charge of.  The display itself will
 * not be deallocated.
 */
void kcr_display_uninit(struct KCR_Display* display);

/*
 * Perform any actions on the display that need to occur before rendering occurs, such as clearing out
 * the previous frame's contents.
 */
void kcr_display_begin_frame(struct KCR_Display* display);

/*
 * Performs actions on the display after all rendering calls have been made, and shows the renderings
 * on the screen.
 */
void kcr_display_finish_frame(struct KCR_Display* display);

/*
 * Helper method to get the index on the pixel buffer for the specified x and y coordinate
 */
int kcr_display_get_pixel_index(const struct KCR_Display* display, int x, int y);

#endif //CRENDERER_DISPLAY_H
