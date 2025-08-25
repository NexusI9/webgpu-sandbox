#ifndef _RENDER_PASS_DRAW_H_
#define _RENDER_PASS_DRAW_H_

#include "core.h"
#include "webgpu/webgpu.h"

void render_pass_list_draw_onscreen_monosample(RenderPassList *);
void render_pass_list_draw_onscreen_multisample(RenderPassList *);
void render_pass_list_draw_offscreen(RenderPassList *);

/* Draw callbacks */
void render_pass_draw_onscreen_monosample(RenderPass *,
                                          const RenderPassViewOverride *);
void render_pass_draw_onscreen_multisample(RenderPass *,
                                           const RenderPassViewOverride *);
void render_pass_draw_offscreen(RenderPass *, const RenderPassViewOverride *);

void render_pass_draw(RenderPass *, const RenderPassViewOverride *);

void render_pass_command_begin(RenderPass *);
void render_pass_command_draw(RenderPass *, const RenderPassViewOverride *);
void render_pass_command_end(RenderPass *);
void render_pass_list_command_end(RenderPassList *, WGPUCommandEncoder);

static inline void render_pass_list_draw(RenderPassList *list) {
  list->draw_callback(list);
}

#endif
