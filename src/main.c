#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"

bool is_running;

bool setup(void) {
    is_running = initialize_display();
    return is_running;
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;

            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }

                break;
        }
    }
}

void update(void) {

}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            pixel_buffer[current_y * window_width + current_x] = color;
        }
    }
}

void render(void) {
    begin_render_cycle();

    draw_rect(100, 100, 50, 50, 0xFFFF0000);

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
