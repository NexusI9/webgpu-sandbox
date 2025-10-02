#ifndef _SCENE_EDITOR_CAMERA_H_
#define _SCENE_EDITOR_CAMERA_H_

#include "runtime/camera/camera.h"

#include <cglm/types.h>

#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"
#include "runtime/camera/core.h"

void sem_camera_create(SceneEditorMeshList *, Camera *,
                       const SEMCreateDescriptor *);

void sem_camera_set_position(SEMTransformCallback *);
void sem_camera_set_rotation(SEMTransformCallback *);
void sem_camera_set_scale(SEMTransformCallback *);

void sem_camera_lookat(SceneEditorMeshList *, vec3, vec3);
void sem_camera_fov(SceneEditorMeshList *, float);

#endif
