#include <stdbool.h>
#include "gfx/display.h"
#include "input.h"
#include "scene/scene.h"

bool isRunning;
struct KCR_InputState inputState;
struct KCR_Display* display;
struct KCR_Scene* scene;

bool setup(void) {
    display = kcr_display_init();
    if (display == NULL) {
        return false;
    }

    scene = kcr_scene_init();
    if (scene == NULL) {
        return false;
    }

    return true;
}

void process_input(void) {
    input_update_state(&inputState);

    if (inputState.quit_requested) {
        isRunning = false;
    }
}

void update(void) {
    kcr_scene_update(scene, &inputState);
}

void render(void) {
    kcr_display_begin_frame(display);
    kcr_scene_render(scene, display);
    kcr_display_finish_frame(display);
}

int main(int argc, char* args[]) {
    isRunning = setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }

    kcr_scene_free(scene);
    kcr_display_free(display);

    return 0;
}
