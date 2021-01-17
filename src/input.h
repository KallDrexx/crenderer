#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

typedef struct {
    bool quit_requested;
} InputState;

void process_events(InputState* state);

#endif //CRENDERER_INPUT_H
