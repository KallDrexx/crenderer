#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "gfx/display.h"
#include "input.h"
#include "scene/scene.h"
#include "gfx/renderer.h"

#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS)

bool isRunning;
struct KCR_InputState inputState;
struct KCR_Display display;
struct KCR_Scene scene;
struct KCR_Renderer renderer;

bool setup(void) {
    if (kcr_display_init(&display) == false) {
        fprintf(stderr, "Failed to initialize display\n");
        return false;
    }

    if (kcr_scene_init(&scene, &display) == false) {
        fprintf(stderr, "Failed to initialize scene\n");
        return false;
    }

    if (kcr_renderer_init(&renderer, &display) == false) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return false;
    }

    return true;
}

void process_input(void) {
    kcr_input_update_state(&inputState);

    if (inputState.quit_requested) {
        isRunning = false;
    }
}

void update(float timeDelta, struct KCR_Display* display) {
    kcr_scene_update(&scene, &inputState, timeDelta, display);
}

void render(void) {
    kcr_display_begin_frame(&display);
    kcr_renderer_render(&renderer, &scene, &inputState);
    kcr_display_finish_frame(&display);
}

int main(__attribute__((unused)) int argc, __attribute__((unused))char *argv[]) {
    isRunning = setup();

    uint32_t previousFrameTime = 0;
    while (isRunning) {
        int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - previousFrameTime);
        if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
            SDL_Delay(timeToWait);
        }

        float timeDelta = (SDL_GetTicks() - previousFrameTime) / 1000.0f;
        previousFrameTime = SDL_GetTicks();

        process_input();
        update(timeDelta, &display);
        render();
    }

    kcr_scene_uninit(&scene);
    kcr_display_uninit(&display);
    kcr_renderer_uninit(&renderer);

    return 0;
}
