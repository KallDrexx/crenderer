#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

struct KCR_InputState {
    bool quit_requested;
    bool left_down;
    bool right_down;
    bool up_down;
    bool down_down;
    bool home_down;
    bool end_down;
    bool left_mouse_down;
    int32_t mouse_drag_x;
    int32_t mouse_drag_y;
    bool minus_down;
    bool equals_down;
    bool f1_pressed;
    bool f2_pressed;
};

void kcr_input_update_state(struct KCR_InputState* state);

#endif //CRENDERER_INPUT_H
