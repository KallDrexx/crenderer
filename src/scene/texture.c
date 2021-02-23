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

            memcpy(texture->texels, upng_get_buffer(png), upng_get_size(png));
        }

        upng_free(png);
    }

    return texture;
}

void kcr_texture_free(struct KCR_Texture* texture) {
    free(texture);
}

