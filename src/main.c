#include <stdbool.h>
#include "display.h"
#include "input.h"

bool is_running;
struct Input_State input_state;

bool setup(void) {
    is_running = initialize_display();
    return is_running;
}

void process_input(void) {
    process_events(&input_state);

    if (input_state.quit_requested) {
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
