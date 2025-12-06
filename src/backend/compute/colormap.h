#ifndef _COMPUTE_COLOR_MAP_H_
#define _COMPUTE_COLOR_MAP_H_

#include "./core.h"
#include "runtime/texture/core.h"
#include "utils/defines.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef enum {
  ColormapStatus_Success,
  ColormapStatus_OutOfBound,
  ColormapStatus_InitFail,
  ColormapStatus_UndefError,
} ColormapStatus;

static const int COLORMAP_WORKGROUP = 8;
static const int COLORMAP_BUFFER_UNIFORM = 0;
static const int COLORMAP_MAX_COLORS = 32;

typedef struct {
  vec4 colors[COLORMAP_MAX_COLORS];
  uint32_t count;
  uint32_t _pad[3];
} ColormapUniform;

EXTERN_C_BEGIN

ColormapStatus compute_pass_colormap_create(ComputePass *,
                                            const ComputePassDescriptor *);

ColormapStatus compute_pass_colormap_update_source_texture(ComputePass *,
                                                           const WGPUTexture);

ColormapStatus compute_pass_colormap_set_colors(ComputePass *,
                                                const ColormapUniform *);

static inline void compute_pass_colormap_destroy(ComputePass *pass) {
  compute_pass_destroy(pass);
}

ColormapStatus
compute_pass_colormap_draw(ComputePass *); // builtin command encoder

ColormapStatus compute_pass_colormap_draw_inline(ComputePass *,
                                                 const WGPUCommandEncoder);

EXTERN_C_END
#endif
