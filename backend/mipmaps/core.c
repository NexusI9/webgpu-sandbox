#include "core.h"
#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>


MipmapsStatus mipmaps_create(WGPUTexture *texture,
                             const MipmapsCreateDescriptor *desc) {

  uint8_t array_layer_count =
      desc->dimension == WGPUTextureViewDimension_2D ? 1 : 6;

  uint32_t mip_count = floorf(log2f(glm_max(desc->width, desc->height))) + 1;

  

  return MipmapsStatus_Success;
}
