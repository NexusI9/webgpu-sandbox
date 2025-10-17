#ifndef _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/light.h"
#include "runtime/light/list.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Base */
void sem_spot_light_create(SceneEditorMeshList *, SpotLight *,
                           const SEMCreateDescriptor *);

// accessor
void sem_list_spot_light_get_position(SceneEditorMeshList *, vec3);
void sem_list_spot_light_get_rotation(SceneEditorMeshList *, vec3);
void sem_list_spot_light_get_scale(SceneEditorMeshList *, vec3);

// mutator (general usage)
void sem_list_spot_light_set_position(SceneEditorMeshList *, vec3);
void sem_list_spot_light_set_rotation(SceneEditorMeshList *, vec3);
void sem_list_spot_light_set_scale(SceneEditorMeshList *, vec3);

// mutator (used in gizmo)
void sem_spot_light_set_position(SceneEditorMesh *, vec3);
void sem_spot_light_set_rotation(SceneEditorMesh *, vec3);
void sem_spot_light_set_scale(SceneEditorMesh *, vec3);

void sem_spot_light_update_transform_callback(SceneEditorMeshList *,
                                              const LightCreateFlag);

/* Shadow (update shadow pass on transform) */

void sem_spot_light_shadow_create(SceneEditorMeshList *, SpotLight *,
                                  const SEMCreateDescriptor *);

// mutator (general usage)
void sem_list_spot_light_shadow_set_position(SceneEditorMeshList *, vec3);
void sem_list_spot_light_shadow_set_rotation(SceneEditorMeshList *, vec3);
void sem_list_spot_light_shadow_set_scale(SceneEditorMeshList *, vec3);

// mutator (used in gizmo)
void sem_spot_light_shadow_set_position(SceneEditorMesh *, vec3);
void sem_spot_light_shadow_set_rotation(SceneEditorMesh *, vec3);
void sem_spot_light_shadow_set_scale(SceneEditorMesh *, vec3);

#ifdef __cplusplus
}
#endif

#endif
