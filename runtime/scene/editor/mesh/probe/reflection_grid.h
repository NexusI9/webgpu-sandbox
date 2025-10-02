#ifndef _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_GRID_H_
#define _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_GRID_H_

#include "runtime/probe/probe.h"
#include "runtime/scene/core.h"
#include "runtime/probe/reflection/grid.h"

void sem_probe_reflection_grid_create(SceneEditorMeshList *, ProbeReflectionGrid *,
                                 const SEMCreateDescriptor *);

void sem_probe_reflection_grid_bound_set_position(SEMTransformCallback *);
void sem_probe_reflection_grid_bound_set_scale(SEMTransformCallback *);

void sem_probe_reflection_grid_set_position(SEMTransformCallback *);
void sem_probe_reflection_grid_set_rotation(SEMTransformCallback *);
void sem_probe_reflection_grid_set_scale(SEMTransformCallback *);

#endif
