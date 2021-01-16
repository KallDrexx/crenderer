#include <stdbool.h>
#include "display.h"
#include "input.h"

bool is_running;

bool setup(void) {
    is_running = initialize_display();
    return is_running;
}

void process_input(void) {
    struct Input_State state;
    state = process_events();

    if (state.quit_requested) {
        is_running = false;
    }
}

void update(void) {

}

void render(void) {
    begin_render_cycle();

    finish_render_cycle();
}

int main(int argc, char* args[]) {
    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    free_display_resources();

    return 0;
}
