#ifndef _SCENE_DRAW_H_
#define _SCENE_DRAW_H_

#include "core.h"

void scene_draw_texture(Scene *, WGPURenderPassEncoder *);
void scene_draw_shadow(Scene *, WGPURenderPassEncoder *);
void scene_draw_solid(Scene *, WGPURenderPassEncoder *);
void scene_draw_wireframe(Scene *, WGPURenderPassEncoder *);
void scene_draw_boundbox(Scene *, WGPURenderPassEncoder *);
void scene_draw_fixed(Scene *, WGPURenderPassEncoder *);
void scene_draw_selection(Scene *, WGPURenderPassEncoder *);

#endif
