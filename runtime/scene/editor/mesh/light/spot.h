#ifndef _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_

#include "runtime/light/light.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"

/* Base */
void sem_spot_light_create(SceneEditorMeshList *, SpotLight *,
                           const SEMCreateDescriptor *);

void sem_spot_light_set_position(SEMTransformCallback *);
void sem_spot_light_set_rotation(SEMTransformCallback *);
void sem_spot_light_set_scale(SEMTransformCallback *);

void sem_spot_light_update_transform_callback(SceneEditorMeshList *,
                                              const LightShadow);

/* Shadow */

void sem_spot_light_shadow_create(SceneEditorMeshList *, SpotLight *,
                                  const SEMCreateDescriptor *);

void sem_spot_light_shadow_set_position(SEMTransformCallback *);
void sem_spot_light_shadow_set_rotation(SEMTransformCallback *);

#endif
