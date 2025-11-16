#ifndef _SCENE_STAT_H_
#define _SCENE_STAT_H_

#include "core.h"

EXTERN_C_BEGIN


static inline void scene_stat_update_vertex_count(Scene *scene,
                                                  const size_t count) {
  stat_update_count(&scene->stats, StatCount_Vertex, count);
}

static inline void scene_stat_update_shader_count(Scene *scene,
                                                  const size_t count) {
  stat_update_count(&scene->stats, StatCount_Shader, count);
}

static inline void scene_stat_update_texture_count(Scene *scene,
                                                   const size_t count) {
  stat_update_count(&scene->stats, StatCount_Texture, count);
}


static inline void scene_stat_update_draw_call_count(Scene *scene,
                                                     const size_t count) {
  stat_update_count(&scene->stats, StatCount_DrawCall, count);
}

EXTERN_C_END

#endif
