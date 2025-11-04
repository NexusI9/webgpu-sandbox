#ifndef _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"

EXTERN_C_BEGIN

/* Base */
void sem_sun_light_create(SceneEditorMeshList *, SunLight *,
                          const SEMCreateDescriptor *);

void sem_sun_light_shadow_create(SceneEditorMeshList *, SunLight *,
                                 const SEMCreateDescriptor *);

void sem_list_sun_light_get_position(SceneEditorMeshList *, vec3);
void sem_list_sun_light_get_rotation(SceneEditorMeshList *, vec3);
void sem_list_sun_light_get_scale(SceneEditorMeshList *, vec3);
EXTERN_C_END

#endif
