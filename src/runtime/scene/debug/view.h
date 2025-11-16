#ifndef _SCENE_DEBUG_VIEW_H_
#define _SCENE_DEBUG_VIEW_H_

#include <webgpu/webgpu.h>

#include "core.h"

#define VIEW_MARGIN 10

EXTERN_C_BEGIN

void scene_debug_view_create(SceneDebug *, const WGPUTextureView);

EXTERN_C_END


#endif
