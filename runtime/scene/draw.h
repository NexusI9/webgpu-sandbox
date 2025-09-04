#ifndef _SCENE_DRAW_H_
#define _SCENE_DRAW_H_

#include "./core.h"

void scene_set_draw_mode(Scene *, const SceneRendererDrawMode);

void scene_update_shadow_map(Scene *, const LightType);

#endif
