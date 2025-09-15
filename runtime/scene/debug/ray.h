#ifndef _SCENE_DEBUG_RAY_H_
#define _SCENE_DEBUG_RAY_H_

#include "core.h"
#include "../utils/color.h"

void scene_debug_ray_create(SceneDebug *, Mesh **);
void scene_debug_ray_add_point(Mesh *, vec3, vec3, color);
void scene_debug_ray_build(SceneDebug *, Mesh *);

#endif
