#ifndef _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_

#include "runtime/light/light.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"

/* base */
void sem_point_light_create(SceneEditorMeshList *, PointLight *,
                            const SEMCreateDescriptor *);

void sem_point_light_set_position(SEMTransformCallback *);
void sem_point_light_set_rotation(SEMTransformCallback *);
void sem_point_light_set_scale(SEMTransformCallback *);

void sem_point_light_update_transform_callback(SceneEditorMeshList *,
                                               const LightShadow);

/* shadow  */

void sem_point_light_shadow_create(SceneEditorMeshList *, PointLight *,
                                   const SEMCreateDescriptor *);

void sem_point_light_shadow_set_position(SEMTransformCallback *);

#endif
