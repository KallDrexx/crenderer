#include <stdio.h>
#include <SDL2/SDL.h>
#include "display.h"

int windowWidth;
int windowHeight;
uint32_t* pixelBuffer;
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

    windowWidth = display_mode.w;
    windowHeight = display_mode.h;

    Uint32 window_flags = SDL_WINDOW_BORDERLESS;
    window = SDL_CreateWindow(
            "CRenderer",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
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

    pixelBuffer = malloc(sizeof(uint32_t) * windowHeight * windowWidth);
    if (pixelBuffer == NULL) {
        fprintf(stderr, "Error allocating pixel buffer\n");

        return false;
    }

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            windowWidth,
            windowHeight);

    if (texture == NULL) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());

        return false;
    }

    return true;
}

void free_display_resources(void) {
    free(pixelBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clear_pixel_buffer(uint32_t color) {
    for (int x = 0; x < windowWidth * windowHeight; x++) {
        pixelBuffer[x] = color;
    }
}

void begin_render_cycle(void) {
    clear_pixel_buffer(0xFF000000);
}

void finish_render_cycle(void) {

    SDL_UpdateTexture(texture, NULL, pixelBuffer, (int)(windowWidth * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);
}

int get_pixel_index(int x, int y) {
    return y * windowWidth + x;
}