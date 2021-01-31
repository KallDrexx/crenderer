#include <SDL2/SDL.h>
#include "input.h"

void handle_keyup_event(struct KCR_InputState *state, SDL_Event *event) {
    switch ((*event).key.keysym.sym) {
        case SDLK_ESCAPE:
            state->quit_requested = true;
            break;

        case SDLK_LEFT:
            state->left_down = false;
            break;

        case SDLK_RIGHT:
            state->right_down = false;
            break;

        case SDLK_UP:
            state->up_down = false;
            break;

        case SDLK_DOWN:
            state->down_down = false;
            break;

        case SDLK_HOME:
            state->home_down = false;
            break;

        case SDLK_END:
            state->end_down = false;
            break;

        case SDLK_F1:
            state->f1_pressed = true; // only trigger on release
            break;

        case SDLK_F2:
            state->f2_pressed = true; // only trigger on release
            break;

        case SDLK_MINUS:
            state->minus_down = false;
            break;

        case SDLK_EQUALS:
            state->equals_down = false;
            break;

        case SDLK_SPACE:
            state->space_pressed = true;
            break;
    }
}

void handle_keydown_event(struct KCR_InputState *state, SDL_Event *event) {
    switch ((*event).key.keysym.sym) {
        case SDLK_LEFT:
            state->left_down = true;
            break;

        case SDLK_RIGHT:
            state->right_down = true;
            break;

        case SDLK_UP:
            state->up_down = true;
            break;

        case SDLK_DOWN:
            state->down_down = true;
            break;

        case SDLK_HOME:
            state->home_down = true;
            break;

        case SDLK_END:
            state->end_down = true;
            break;

        case SDLK_MINUS:
            state->minus_down = true;
            break;

        case SDLK_EQUALS:
            state->equals_down = true;
            break;
    }
}

void kcr_input_update_state(struct KCR_InputState* state) {
    state->f1_pressed = false;
    state->f2_pressed = false;
    state->space_pressed = false;
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
