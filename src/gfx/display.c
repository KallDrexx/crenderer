#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "display.h"

struct KCR_Display_Internal {
    SDL_Texture* texture;
    SDL_Window* window;
    SDL_Renderer* renderer;
};

bool kcr_display_init(struct KCR_Display* display) {
    assert(display != NULL);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL: %s.\n", SDL_GetError());
        return false;
    }

    display->internal = malloc(sizeof(struct KCR_Display_Internal));;

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    display->windowWidth = display_mode.w;
    display->windowHeight = display_mode.h;

    uint32_t window_flags = SDL_WINDOW_BORDERLESS;
    display->internal->window = SDL_CreateWindow(
            "CRenderer",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            display->windowWidth,
            display->windowHeight,
            window_flags);

    if (!display->internal->window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        kcr_display_uninit(display);

        return false;
    }

    display->internal->renderer = SDL_CreateRenderer(display->internal->window, -1, 0);
    if (!display->internal->renderer) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        kcr_display_uninit(display);

        return false;
    }

    display->pixelBuffer = malloc(sizeof(uint32_t) * display->windowHeight * display->windowWidth);
    if (display->pixelBuffer == NULL) {
        fprintf(stderr, "Error allocating pixel buffer\n");
        kcr_display_uninit(display);

        return false;
    }

    display->internal->texture = SDL_CreateTexture(
            display->internal->renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            display->windowWidth,
            display->windowHeight);

    if (display->internal->texture == NULL) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
        kcr_display_uninit(display);

        return false;
    }

    return display;
}

void kcr_display_uninit(struct KCR_Display* display) {
    if (display != NULL) {
        if (display->internal != NULL) {
            // SDL_DestroyRenderer destroys associated textures, so we shouldn't do that ourselves
            SDL_DestroyRenderer(display->internal->renderer);
            SDL_DestroyWindow(display->internal->window);
            free(display->internal);
            display->internal = NULL;
        }

        free(display->pixelBuffer);
        display->pixelBuffer = NULL;
    }
}

void kcr_display_begin_frame(struct KCR_Display* display) {
    for (int x = 0; x < display->windowWidth * display->windowHeight; x++) {
        display->pixelBuffer[x] = 0xFF000000;
    }
}

void kcr_display_finish_frame(struct KCR_Display* display) {

    SDL_UpdateTexture(
            display->internal->texture,
            NULL,
            display->pixelBuffer,
            (int)(display->windowWidth * sizeof(uint32_t)));

    SDL_RenderCopy(display->internal->renderer, display->internal->texture, NULL, NULL);
    SDL_RenderPresent(display->internal->renderer);
}
