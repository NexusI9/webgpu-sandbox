#ifndef _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_GRID_H_
#define _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_GRID_H_

#include "runtime/probe/probe.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/scene/core.h"

void sem_probe_reflection_grid_create(SceneEditorMeshList *,
                                      ProbeReflectionGrid *,
                                      const SEMCreateDescriptor *);

void sem_probe_reflection_grid_bound_set_position(SceneEditorMesh *, vec3);
void sem_probe_reflection_grid_bound_set_scale(SceneEditorMesh *, vec3);

void sem_probe_reflection_grid_set_position(SceneEditorMesh *, vec3);
void sem_probe_reflection_grid_set_rotation(SceneEditorMesh *, vec3);
void sem_probe_reflection_grid_set_scale(SceneEditorMesh *, vec3);

#endif
