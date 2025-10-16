#include "backend/stat.h"
#include "backend/registry.h"
#include "runtime/geometry/vertex/core.h"
#include "runtime/pipeline/render.h"
#include "stat.h"

/**
   Get the number of drawn vertex in the current active scene render pass list.
 */
void scene_stat_update_vertex_count(Scene *scene) {

  int count = 0;

  RenderPassList *active_list =
      scene_renderer_pass_list(&scene->renderer, scene->renderer.draw.mode);

  float denom = 1.0f / VERTEX_STRIDE;

  for (size_t i = 0; i < active_list->length; i++)
    for (size_t j = 0; j < active_list->passes[i].draw_list.length; j++)
      for (size_t k = 0;
           k < active_list->passes[i].draw_list.entries[j].drawn_meshes.length;
           k++)
        count += active_list->passes[i]
                     .draw_list.entries[j]
                     .drawn_meshes.entries[k]
                     ->topology.base.attribute.length *
                 denom;

  stat_update_count(&scene->renderer.stats, StatCount_Vertex, count);
}

/**
   Get the number of render and compute pipelines from the registry
 */
void scene_stat_update_shader_count(Scene *scene) {

  int count = 0;

  for (size_t i = 0; i < reg_length(); i++) {
    const RegEntry *entry = reg_lookup(i);
    if (entry->type == RegEntryType_RenderPipeline ||
        entry->type == RegEntryType_ComputePipeline)
      count++;
  }

  stat_update_count(&scene->renderer.stats, StatCount_Shader, count);
}

/**
   Get the number of textures entities from the registry
 */
void scene_stat_update_texture_count(Scene *scene) {

  int count = 0;

  for (size_t i = 0; i < reg_length(); i++) {
    const RegEntry *entry = reg_lookup(i);
    if (entry->type == RegEntryType_Texture)
      count++;
  }

  stat_update_count(&scene->renderer.stats, StatCount_Texture, count);
}

/**
   Get the number of drawn meshes in the current active scene render pass list.
 */
void scene_stat_update_draw_call_count(Scene *scene) {

  int count = 0;
  RenderPassList *active_list =
      scene_renderer_pass_list(&scene->renderer, scene->renderer.draw.mode);

  for (size_t i = 0; i < active_list->length; i++)
    for (size_t j = 0; j < active_list->passes[i].draw_list.length; j++)
      count += active_list->passes[i].draw_list.entries[j].drawn_meshes.length;

  stat_update_count(&scene->renderer.stats, StatCount_DrawCall, count);
}
