#ifndef _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"

EXTERN_C_BEGIN

/* base */
void sem_point_light_create(SceneEditorMeshList *, PointLight *,
                            const SEMCreateDescriptor *);

void sem_point_light_shadow_create(SceneEditorMeshList *, PointLight *,
                                   const SEMCreateDescriptor *);

// accessor
void sem_list_point_light_get_position(SceneEditorMeshList *, vec3);
void sem_list_point_light_get_rotation(SceneEditorMeshList *, vec3);
void sem_list_point_light_get_scale(SceneEditorMeshList *, vec3);


EXTERN_C_END

#endif
