#include <SDL2/SDL.h>
#include "input.h"

struct Input_State process_events() {
    struct Input_State results;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
                results.quit_requested = true;
                break;

            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    results.quit_requested = true;
                }

                break;
        }
    }

    return results;
}
