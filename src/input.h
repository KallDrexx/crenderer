#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

struct KCR_InputState {
    bool quit_requested : 1;
    bool left_down : 1;
    bool right_down : 1;
    bool up_down : 1;
    bool down_down : 1;
    bool home_down : 1;
    bool end_down : 1;
    bool left_mouse_down : 1;
    bool minus_down : 1;
    bool equals_down : 1;
    bool f1_pressed : 1;
    bool f2_pressed : 1;
    bool space_pressed : 1;
    bool one_pressed : 1;
    bool two_pressed : 1;
    bool three_pressed : 1;
    bool four_pressed : 1;
    bool five_pressed : 1;
    bool six_pressed : 1;
    bool seven_pressed : 1;
    bool eight_pressed : 1;
    bool nine_pressed : 1;
    bool zero_pressed : 1;
    bool c_pressed : 1;
    bool page_down_down : 1;
    bool page_up_down : 1;
    int32_t mouse_drag_x;
    int32_t mouse_drag_y;
};

void kcr_input_update_state(struct KCR_InputState* state);

#endif //CRENDERER_INPUT_H
