#ifndef _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"

EXTERN_C_BEGIN

void sem_ambient_light_create(SceneEditorMeshList *, AmbientLight *,
                              const SEMCreateDescriptor *);

// accessor
void sem_list_ambient_light_get_position(SceneEditorMeshList *, vec3);
void sem_list_ambient_light_get_rotation(SceneEditorMeshList *, vec3);
void sem_list_ambient_light_get_scale(SceneEditorMeshList *, vec3);

EXTERN_C_END

#endif
