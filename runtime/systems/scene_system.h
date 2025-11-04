#ifndef _SCENE_SYSTEM_H_
#define _SCENE_SYSTEM_H_

#include "backend/renderer/core.h"
#include "runtime/scene/core.h"

void scene_system_set_draw_mode(Scene *, Renderer *,
                                      const RendererDrawMode);

void scene_system_scene_render_pass_texture(
    Scene *, Renderer *, int, int, const RenderPipelineMultisampleCount,
    const double);

static inline void
scene_system_set_scene_post_fx_bloom(Scene *scene, Renderer *rd,
                                     const BloomUniform bloom) {

  // retrieve the pass fx of the last texture pass
  // TODO: make the access more easy; rn not intuitive
  PostFx *texture_pass_fx =
      &render_pass_list_last_pass(
           renderer_mode_mesh_pass_list(rd, RendererDrawMode_Texture))
           ->post_fx;

  PostFxEffectUniform fx_uniform = {.bloom = bloom};
  post_fx_bloom_update_uniform(texture_pass_fx, fx_uniform);
}

static inline void
scene_system_set_scene_post_fx(Scene *scene, Renderer *rd,
                               const CompositeUniform composite) {
  // retrieve the pass fx of the last texture pass
  PostFx *texture_pass_fx =
      &render_pass_list_last_pass(
           renderer_mode_mesh_pass_list(rd, RendererDrawMode_Texture))
           ->post_fx;

  PostFxEffectUniform fx_uniform = {.composite = composite};
  post_fx_composite_update_uniform(texture_pass_fx, fx_uniform);
}

/**
   Get the number of drawn vertex in the current active scene render pass list.
 */
static inline size_t scene_system_get_scene_vertex_count(Scene *scene,
                                                         Renderer *rd) {

  int count = 0;

  RenderPassList *active_list = renderer_mesh_pass_list(rd, rd->draw_mode);

  float denom = 1.0f / 3;

  for (size_t i = 0; i < active_list->length; i++)
    for (size_t j = 0; j < active_list->passes[i].draw_list.length; j++)
      for (size_t k = 0;
           k < active_list->passes[i].draw_list.entries[j].drawn_meshes.length;
           k++)
        count += active_list->passes[i]
                     .draw_list.entries[j]
                     .drawn_meshes.entries[k]
                     .length *
                 denom;

  return count;
}

/**
   Get the number of render and compute pipelines from the registry
 */
static inline size_t scene_system_get_scene_shader_count(Scene *scene,
                                                         Renderer *rd) {
  int count = 0;

  for (size_t i = 0; i < reg_length(); i++) {
    const RegEntry *entry = reg_lookup(i);
    if (entry->type == RegEntryType_RenderPipeline ||
        entry->type == RegEntryType_ComputePipeline)
      count++;
  }

  return count;
}

/**
   Get the number of textures entities from the registry
 */
static inline size_t renderer_get_scene_texture_count(Scene *scene,
                                                      Renderer *rd) {

  int count = 0;

  for (size_t i = 0; i < reg_length(); i++) {
    const RegEntry *entry = reg_lookup(i);
    if (entry->type == RegEntryType_Texture)
      count++;
  }

  return count;
}

/**
   Get the number of drawn meshes in the current active scene render pass list.
 */
static inline size_t renderer_get_scene_draw_call_count(Scene *scene,
                                                        Renderer *rd) {

  int count = 0;
  RenderPassList *active_list = renderer_mesh_pass_list(rd, rd->draw_mode);

  for (size_t i = 0; i < active_list->length; i++)
    for (size_t j = 0; j < active_list->passes[i].draw_list.length; j++)
      count += active_list->passes[i].draw_list.entries[j].drawn_meshes.length;

  return count;
}

static inline RendererStatus renderer_draw_scene(Scene *scene, Renderer *rd) {
  // FIXME
  // Maybe don't use add/remove system to ensure drawing 1 scene at a time
  renderer_add_draw_callback(rd, ubo_draw_callback, (void *)scene->ubo,
                             RendererDrawMode_Texture | RendererDrawMode_Solid |
                                 RendererDrawMode_Wireframe |
                                 RendererDrawMode_Boundbox);

  renderer_add_draw_callback(rd, renderer_draw_layout_callback, (void *)rd,
                             RendererDrawMode_Texture | RendererDrawMode_Solid |
                                 RendererDrawMode_Wireframe |
                                 RendererDrawMode_Boundbox);

  return RendererStatus_Success;
}

#endif
