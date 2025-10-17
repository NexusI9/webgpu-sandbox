#ifndef _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/light.h"
#include "runtime/light/list.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Base */
void sem_sun_light_create(SceneEditorMeshList *, SunLight *,
                          const SEMCreateDescriptor *);

// accessor
void sem_list_sun_light_get_position(SceneEditorMeshList *, vec3);
void sem_list_sun_light_get_rotation(SceneEditorMeshList *, vec3);
void sem_list_sun_light_get_scale(SceneEditorMeshList *, vec3);

// mutator (general usage)
void sem_list_sun_light_set_position(SceneEditorMeshList *, vec3);
void sem_list_sun_light_set_rotation(SceneEditorMeshList *, vec3);
void sem_list_sun_light_set_scale(SceneEditorMeshList *, vec3);

// mutator (used in gizmo)
void sem_sun_light_set_position(SceneEditorMesh *, vec3);
void sem_sun_light_set_rotation(SceneEditorMesh *, vec3);
void sem_sun_light_set_scale(SceneEditorMesh *, vec3);

void sem_sun_light_update_transform_callback(SceneEditorMeshList *,
                                             const LightCreateFlag);

/* Shadow (update shadow pass on transform) */
void sem_sun_light_shadow_create(SceneEditorMeshList *, SunLight *,
                                 const SEMCreateDescriptor *);

// mutator (general usage)
void sem_list_sun_light_shadow_set_position(SceneEditorMeshList *, vec3);
void sem_list_sun_light_shadow_set_rotation(SceneEditorMeshList *, vec3);
void sem_list_sun_light_shadow_set_scale(SceneEditorMeshList *, vec3);

// mutator (used in gizmo)
void sem_sun_light_shadow_set_position(SceneEditorMesh *, vec3);
void sem_sun_light_shadow_set_rotation(SceneEditorMesh *, vec3);
void sem_sun_light_shadow_set_scale(SceneEditorMesh *, vec3);

#ifdef __cplusplus
}
#endif

#endif
