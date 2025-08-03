#ifndef _SCENE_EDITOR_CALLBACK_TRANSFORM_H_
#define _SCENE_EDITOR_CALLBACK_TRANSFORM_H_

#include "../../core.h"
#include <cglm/cglm.h>

/* Mesh based transform */
void scene_selection_mesh_translate(MeshRefList *, Vec3List*, vec3, const Axis);
void scene_selection_mesh_rotate(MeshRefList *, Vec3List*, vec3, const Axis);
void scene_selection_mesh_scale(MeshRefList *, Vec3List*, vec3, const Axis);

/* Mesh based transform */
void scene_selection_shader_translate(MeshRefList *, Vec3List*, vec3, const Axis);
void scene_selection_shader_rotate(MeshRefList *, Vec3List*, vec3, const Axis);
void scene_selection_shader_scale(MeshRefList *, Vec3List*, vec3, const Axis);

#endif
