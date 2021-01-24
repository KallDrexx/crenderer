#include <SDL2/SDL.h>
#include "input.h"

void handle_keyup_event(struct KCR_InputState *state, SDL_Event *event) {
    switch ((*event).key.keysym.sym) {
        case SDLK_ESCAPE:
            state->quit_requested = true;
            break;

        case SDLK_LEFT:
            state->left_pressed = false;
            break;

        case SDLK_RIGHT:
            state->right_pressed = false;
            break;

        case SDLK_UP:
            state->up_pressed = false;
            break;

        case SDLK_DOWN:
            state->down_pressed = false;
            break;

        case SDLK_HOME:
            state->home_pressed = false;
            break;

        case SDLK_END:
            state->end_pressed = false;
            break;
    }
}

void handle_keydown_event(struct KCR_InputState *state, SDL_Event *event) {
    switch ((*event).key.keysym.sym) {
        case SDLK_LEFT:
            state->left_pressed = true;
            break;

        case SDLK_RIGHT:
            state->right_pressed = true;
            break;

        case SDLK_UP:
            state->up_pressed = true;
            break;

        case SDLK_DOWN:
            state->down_pressed = true;
            break;

        case SDLK_HOME:
            state->home_pressed = true;
            break;

        case SDLK_END:
            state->end_pressed = true;
            break;
    }
}

void kcr_input_update_state(struct KCR_InputState* state) {
    state->mouse_drag_y = 0;
    state->mouse_drag_x = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
                state->quit_requested = true;
                break;

            case SDL_KEYUP:
                handle_keyup_event(state, &event);
                break;

            case SDL_KEYDOWN:
                handle_keydown_event(state, &event);
                break;

            case SDL_MOUSEMOTION:
                state->mouse_drag_x = event.motion.xrel;
                state->mouse_drag_y = event.motion.yrel;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    state->left_mouse_down = true;
                }

                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    state->left_mouse_down = false;
                }
        }
    }
}
