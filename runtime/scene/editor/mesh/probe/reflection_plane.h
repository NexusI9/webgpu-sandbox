#ifndef _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_PLANE_H_
#define _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_PLANE_H_

#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/core.h"

void sem_probe_reflection_plane_create(SceneEditorMeshList *,
                                       ProbeReflectionPlane *,
                                       const SEMCreateDescriptor *);

void sem_probe_reflection_plane_set_position(SceneEditorMesh *, vec3);
void sem_probe_reflection_plane_set_rotation(SceneEditorMesh *, vec3);
void sem_probe_reflection_plane_set_scale(SceneEditorMesh *, vec3);

void sem_probe_reflection_plane_update_mesh_uniform(SceneEditorMesh *);

#endif
