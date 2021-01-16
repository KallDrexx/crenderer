#include <stdio.h>
#include <SDL2/SDL.h>
#include "display.h"

int window_width;
int window_height;
uint32_t* pixel_buffer;
SDL_Texture* texture;
SDL_Window* window;
SDL_Renderer* renderer;

bool initialize_display(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL: %s.\n", SDL_GetError());
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w;
    window_height = display_mode.h;

    Uint32 window_flags = SDL_WINDOW_BORDERLESS;
    window = SDL_CreateWindow(
            "CRenderer",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            window_flags);

    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        return false;
    }

    pixel_buffer = malloc(sizeof(uint32_t) * window_height * window_width);
    if (pixel_buffer == NULL) {
        fprintf(stderr, "Error allocating pixel buffer\n");

        return false;
    }

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            window_width,
            window_height);

    if (texture == NULL) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());

        return false;
    }

    return true;
}

void free_display_resources(void) {
    free(pixel_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clear_pixel_buffer(uint32_t color) {
    for (int x = 0; x < window_width * window_height; x++) {
        pixel_buffer[x] = color;
    }
}

void begin_render_cycle(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    clear_pixel_buffer(0xFF000000);
}

void finish_render_cycle(void) {

    SDL_UpdateTexture(texture, NULL, pixel_buffer, (int)(window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);
}