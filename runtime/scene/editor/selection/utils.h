#ifndef _SCENE_EDITOR_SELECTION_UTILS_H_
#define _SCENE_EDITOR_SELECTION_UTILS_H_

#include "../../core.h"

void scene_selection_add(Scene *, Mesh *);
void scene_selection_remove(Scene *, Mesh *);
void scene_selection_average_position(Scene *, vec3 *);


#endif
