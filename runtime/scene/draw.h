#ifndef _SCENE_DRAW_H_
#define _SCENE_DRAW_H_

#include "./core.h"
#include "renderer/core.h"
#include "runtime/pipeline/render.h"

EXTERN_C_BEGIN

void scene_set_draw_mode(Scene *, const SceneRendererDrawMode);

void scene_update_render_pass_texture(Scene *, const int, const int,
                                      const RenderPipelineMultisampleCount,
                                      const double);

EXTERN_C_END

#endif
