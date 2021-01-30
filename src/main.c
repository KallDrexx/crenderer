#include <stdlib.h>
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
struct KCR_Display* display;
struct KCR_Scene* scene;

bool setup(void) {
    display = malloc(sizeof(struct KCR_Display));
    bool displayInitialized = kcr_display_init(display);
    if (displayInitialized == false) {
        return false;
    }

    scene = malloc(sizeof(struct KCR_Scene));
    bool sceneInitSuccess = kcr_scene_init(scene);
    if (sceneInitSuccess == false) {
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

void update(float timeDelta) {
    kcr_scene_update(scene, &inputState, timeDelta);
}

void render(void) {
    kcr_display_begin_frame(display);
    kcr_render(display, scene);
    kcr_display_finish_frame(display);
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
        update(timeDelta);
        render();
    }

    kcr_scene_uninit(scene);
    kcr_display_uninit(display);

    return 0;
}
