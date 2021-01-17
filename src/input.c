#include <SDL2/SDL.h>
#include "input.h"

void clear_state(InputState* state) {
    state->quit_requested = false;
}

void process_events(InputState* state) {
    clear_state(state);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
                state->quit_requested = true;
                break;

            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    state->quit_requested = true;
                }

                break;
        }
    }
}
