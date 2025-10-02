#ifndef _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_

#include "runtime/light/light.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"

/* base */
void sem_light_point_create(SceneEditorMeshList *, PointLight *,
                            const SEMCreateDescriptor *);

void sem_light_point_set_position(SEMTransformCallback *);
void sem_light_point_set_rotation(SEMTransformCallback *);
void sem_light_point_set_scale(SEMTransformCallback *);

void sem_light_point_update_transform_callback(SceneEditorMeshList *,
                                               const LightShadow);

/* shadow  */

void sem_light_point_shadow_create(SceneEditorMeshList *, PointLight *,
                                   const SEMCreateDescriptor *);

void sem_light_point_shadow_set_position(SEMTransformCallback *);

#endif
