#ifndef _SCENE_EDITOR_CAMERA_H_
#define _SCENE_EDITOR_CAMERA_H_

#include "runtime/camera/camera.h"

#include <cglm/types.h>

#include "runtime/camera/core.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/core.h"

#ifdef __cplusplus
extern "C" {
#endif

void sem_camera_create(SceneEditorMeshList *, Camera *,
                       const SEMCreateDescriptor *);

// accessor
void sem_list_camera_get_position(SceneEditorMeshList *, vec3);
void sem_list_camera_get_rotation(SceneEditorMeshList *, vec3);
void sem_list_camera_get_scale(SceneEditorMeshList *, vec3);

// mutator (general usage)
void sem_list_camera_set_position(SceneEditorMeshList *, vec3);
void sem_list_camera_set_rotation(SceneEditorMeshList *, vec3);
void sem_list_camera_set_scale(SceneEditorMeshList *, vec3);

// mutator (used in gizmo transform)
void sem_camera_set_position(SceneEditorMesh *, vec3);
void sem_camera_set_rotation(SceneEditorMesh *, vec3);
void sem_camera_set_scale(SceneEditorMesh *, vec3);

void sem_camera_lookat(SceneEditorMeshList *, vec3, vec3);
void sem_camera_fov(SceneEditorMeshList *, float);

#ifdef __cplusplus
}
#endif

#endif
