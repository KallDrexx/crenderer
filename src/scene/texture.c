#include <stdlib.h>
#include <string.h>
#include "texture.h"
#include "upng.h"

struct KCR_Texture *kcr_texture_from_asset(const char *assetName) {
    #define ASSETS_FOLDER "assets/"
    #define EXTENSION ".png"

    assert(assetName != NULL);

    char* filename = calloc(strlen(assetName) + strlen(ASSETS_FOLDER) + strlen(EXTENSION), sizeof(char));
    strcat(filename, ASSETS_FOLDER);
    strcat(filename, assetName);
    strcat(filename, EXTENSION);

    struct KCR_Texture* texture = NULL;
    upng_t* png = upng_new_from_file(filename);
    if (png != NULL) {
        upng_decode(png);
        if (upng_get_error(png) == UPNG_EOK) {
            texture = malloc(sizeof(struct KCR_Texture) + upng_get_size(png));
            texture->width = upng_get_width(png);
            texture->height = upng_get_height(png);

            // Since PNGs are usually RGBA, but all of our current color operations are based on ARGB, we need to
            // convert each texel from the former to the latter.
            const uint8_t* buffer = upng_get_buffer(png);
            for (uint32_t texelIndex = 0; texelIndex < texture->width * texture->height; texelIndex++) {
                uint32_t bufferIndex = texelIndex * 4;

                uint32_t red   = ((uint32_t) buffer[bufferIndex + 0] << 16);
                uint32_t green = ((uint32_t) buffer[bufferIndex + 1] << 8);
                uint32_t blue  = ((uint32_t) buffer[bufferIndex + 2] << 0);
                uint32_t alpha = ((uint32_t) buffer[bufferIndex + 3] << 24);

                texture->texels[texelIndex] = alpha | red | green | blue;
            }
        }

        upng_free(png);
    }

    return texture;
}

void kcr_texture_free(struct KCR_Texture* texture) {
    free(texture);
}

