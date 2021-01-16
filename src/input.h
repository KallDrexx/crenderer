#ifndef CRENDERER_INPUT_H
#define CRENDERER_INPUT_H

#include <stdbool.h>

struct Input_State {
    bool quit_requested;
};

struct Input_State process_events();

#endif //CRENDERER_INPUT_H
