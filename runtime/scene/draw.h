#ifndef _SCENE_DRAW_H_
#define _SCENE_DRAW_H_

#include "./core.h"
#include "renderer/core.h"

#ifdef __cplusplus
extern "C" {
#endif

void scene_set_draw_mode(Scene *, const SceneRendererDrawMode);

#ifdef __cplusplus
}
#endif

#endif
