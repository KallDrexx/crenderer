#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

struct Input_State {
    bool quit_requested;
};

void process_events(struct Input_State* state);

#endif //CRENDERER_INPUT_H
