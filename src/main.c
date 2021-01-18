#include <stdbool.h>
#include "gfx/display.h"
#include "input.h"
#include "scene/scene.h"

bool isRunning;
InputState inputState;

bool setup(void) {
    isRunning = display_init();
    scene_init();

    return true;
}

void process_input(void) {
    input_update_state(&inputState);

    if (inputState.quit_requested) {
        isRunning = false;
    }
}

void update(void) {
    scene_update(&inputState);
}

void render(void) {
    display_begin_frame();

    scene_render();

    display_finish_frame();
}

int main(int argc, char* args[]) {
    setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }

    display_free();

    return 0;
}
