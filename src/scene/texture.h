#ifndef CRENDERER_TEXTURE_H
#define CRENDERER_TEXTURE_H

#include <stdint.h>
#include <assert.h>
#include <math.h>

/*
 * Represents texture data
 */
struct KCR_Texture {
    uint32_t width;
    uint32_t height;
    uint32_t texels[];
};

/*
 * Creates a texture from a constant in memory representation of a red brick texture
 */
struct KCR_Texture* kcr_texture_red_brick(void);

/*
 * Frees a previously allocated texture
 */
void kcr_texture_free(struct KCR_Texture* texture);

/*
 * Find the index for a given texel based on a given u,v coordinate
 */
static inline uint32_t kcr_texture_texel_index(const struct KCR_Texture* texture, float u, float v) {
    assert(texture != NULL);
    assert(u >= 0 && u <= 1.0f);
    assert(v >= 0 && v <= 1.0f);

    uint32_t y = texture->height - (uint32_t)(texture->height * v);
    uint32_t x = (uint32_t) (texture->width * u);

    return y * texture->width + x;
}

#endif //CRENDERER_TEXTURE_H
