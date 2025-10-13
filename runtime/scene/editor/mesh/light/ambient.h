#ifndef _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_

#include "runtime/light/light.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"
#include "runtime/light/core.h"

void sem_ambient_light_create(SceneEditorMeshList *, AmbientLight *,
                              const SEMCreateDescriptor *);

void sem_ambient_light_set_position(SEMTransformCallback *);
void sem_ambient_light_set_rotation(SEMTransformCallback *);
void sem_ambient_light_set_scale(SEMTransformCallback *);



#endif
