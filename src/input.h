#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

struct KCR_InputState {
    bool quit_requested;
    bool left_pressed;
    bool right_pressed;
    bool up_pressed;
    bool down_pressed;
    bool home_pressed;
    bool end_pressed;
    bool left_mouse_down;
    int32_t mouse_drag_x;
    int32_t mouse_drag_y;
};

void kcr_input_update_state(struct KCR_InputState* state);

#endif //CRENDERER_INPUT_H
