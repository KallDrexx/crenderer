#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

struct KCR_InputState {
    bool quit_requested;
};

void input_update_state(struct KCR_InputState* state);

#endif //CRENDERER_INPUT_H
