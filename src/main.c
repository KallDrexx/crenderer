#include <stdbool.h>
#include "gfx/display.h"
#include "input.h"
#include "scene/scene.h"

bool isRunning;
InputState inputState;
struct KCR_Display* display;

bool setup(void) {
    display = kcr_display_init();
    if (display == NULL) {
        return false;
    }

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
    kcr_display_begin_frame(display);
    scene_render(display);
    kcr_display_finish_frame(display);
}

int main(int argc, char* args[]) {
    isRunning = setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }

    kcr_display_free(display);

    return 0;
}
