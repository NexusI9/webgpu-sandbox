#include "core.h"

#include <stdlib.h>

// cache texel size for division are expensive during hot path
float g_texture_resolution_texel_size[TextureResolution_4096 + 1] = {
    [TextureResolution_16] = 1.0f / TextureResolution_16,
    [TextureResolution_32] = 1.0f / TextureResolution_32,
    [TextureResolution_64] = 1.0f / TextureResolution_64,
    [TextureResolution_128] = 1.0f / TextureResolution_128,
    [TextureResolution_256] = 1.0f / TextureResolution_256,
    [TextureResolution_512] = 1.0f / TextureResolution_512,
    [TextureResolution_1024] = 1.0f / TextureResolution_1024,
    [TextureResolution_2048] = 1.0f / TextureResolution_2048,
    [TextureResolution_4096] = 1.0f / TextureResolution_4096,
};

void texture_save(Texture *texture, const char *path) {}

void texture_free(Texture *texture) { free(texture->data); }
