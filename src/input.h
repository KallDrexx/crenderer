#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

typedef struct {
    bool quit_requested;
} InputState;

void input_update_state(InputState* state);

#endif //CRENDERER_INPUT_H
