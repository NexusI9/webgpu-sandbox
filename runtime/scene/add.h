#ifndef _SCENE_ADD_H_
#define _SCENE_ADD_H_

#include "./editor/gizmo/gizmo.h"
#include "core.h"

/**
   Key methods to add, remove, hide or show an item in the scene. Below are some
   key semantic precisions:

   .---------------------------------------------------------------------.
   |                ADD               |              REMOVE              |
   |---------------------------------------------------------------------|
   |   Add and Remove functions basically mounts, unmounts the mesh      |
   |   from the scene. Meaning they build the mesh shader internally     |
   |   and show it visually by adding it to the pipeline list.           |
   |   Those 2 functions should only be used for first and last instan-  |
   |   tiation of the mesh.                                              |
   |                                                                     |
   |   .------------ ⚙ ------------.    .------------ ◉ -------------.  |
   |   |    BUILD    |   UNBUILD    |    |     SHOW    |     HIDE     |  |
   |   |----------------------------|    |----------------------------|  |
   |   | Build and Unbuild function | => | Show and Hide functions    |  |
   |   | only handle the mesh       | => | operate at a visual level  |  |
   |   | internal binding. It does  | => | only. They only pop or push|  |
   |   | not visually add the mesh  | => | the mesh from the pipeline |  |
   |   | to the scene pipeline.     | => | array. However it's        |  |
   |   | Building only "prepares"   | => | important to make sure the |  |
   |   | the mesh for the drawcall. |    | mesh is Built priorly.     |  |
   |   '----------------------------'    '----------------------------'  |
   '---------------------------------------------------------------------'


 */

// Scene Editor Objects
// light
SceneEditorObject *scene_add_point_light(Scene *, PointLightDescriptor *, const LightShadow);
SceneEditorObject *scene_add_spot_light(Scene *, SpotLightDescriptor *, const LightShadow);
SceneEditorObject *scene_add_sun_light(Scene *, SunLightDescriptor *, const LightShadow);
SceneEditorObject *scene_add_ambient_light(Scene *, AmbientLightDescriptor *);

// camera
SceneEditorObject *scene_add_camera(Scene *, const CameraCreateDescriptor *);

// Scene Meshes
Mesh *scene_new_mesh(Scene *);
void scene_add_mesh(Scene *, Mesh *, const char *);
void scene_add_mesh_ref_list(Scene *, MeshRefList *, const char *);

void scene_add_mesh_fixed(Scene *, Mesh *, const ScenePipeline, const char *);
void scene_add_mesh_fixed_ref_list(Scene *, MeshRefList *, const ScenePipeline,
                                   const char *);

void scene_remove_mesh(Scene *, Mesh *, const ScenePipeline);
void scene_remove_mesh_ref_list(Scene *, MeshRefList *, const ScenePipeline);

#endif
