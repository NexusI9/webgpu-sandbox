#ifndef _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_

#include "runtime/light/light.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"

/* Base */
void sem_light_sun_create(SceneEditorMeshList *, SunLight *,
                          const SEMCreateDescriptor *);

void sem_light_sun_set_position(SEMTransformCallback *);
void sem_light_sun_set_rotation(SEMTransformCallback *);
void sem_light_sun_set_scale(SEMTransformCallback *);

void sem_light_sun_update_transform_callback(SceneEditorMeshList *,
                                             const LightShadow);

/* Shadow */
void sem_light_sun_shadow_create(SceneEditorMeshList *, SunLight *,
                                 const SEMCreateDescriptor *);

void sem_light_sun_shadow_set_position(SEMTransformCallback *);

#endif
