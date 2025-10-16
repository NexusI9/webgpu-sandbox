#ifndef _SCENE_STAT_H_
#define _SCENE_STAT_H_

#include "core.h"

#ifdef __cplusplus
extern "C" {
#endif

void scene_stat_update_vertex_count(Scene *);
void scene_stat_update_shader_count(Scene *);
void scene_stat_update_texture_count(Scene *);
void scene_stat_update_draw_call_count(Scene *);

#ifdef __cplusplus
}
#endif

#endif
