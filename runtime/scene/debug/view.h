#ifndef _SCENE_DEBUG_VIEW_H_
#define _SCENE_DEBUG_VIEW_H_

#include <webgpu/webgpu.h>

#include "core.h"

#define VIEW_MARGIN 10

#ifdef __cplusplus
extern "C" {
#endif

void scene_debug_view_create(SceneDebug *, const WGPUTextureView);

#ifdef __cplusplus
}
#endif

#endif
