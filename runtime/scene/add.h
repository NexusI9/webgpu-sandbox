#ifndef _SCENE_ADD_H_
#define _SCENE_ADD_H_

#include "core.h"
#include "runtime/camera/core.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"

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

typedef enum {
  SceneAddFlag_None = 0,
  SceneAddFlag_Hide = 1 << 0,
  SceneAddFlag_Unselectable = 1 << 1,
  SceneAddFlag_TreeHide = 1 << 2,
} SceneAddFlag;

/* === Scene Editor Objects === */

// light
SceneEditorMeshList *scene_add_point_light(Scene *, PointLightDescriptor *,
                                           const LightCreateFlag,
                                           PointLight **);
SceneEditorMeshList *scene_add_spot_light(Scene *, SpotLightDescriptor *,
                                          const LightCreateFlag, SpotLight **);
SceneEditorMeshList *scene_add_sun_light(Scene *, SunLightDescriptor *,
                                         const LightCreateFlag, SunLight **);
SceneEditorMeshList *scene_add_ambient_light(Scene *, AmbientLightDescriptor *,
                                             AmbientLight **);

// Probe
SceneEditorMeshList *
scene_add_probe_reflection_grid(Scene *, ProbeReflectionGridDescriptor *,
                                ProbeReflectionGrid **);

SceneEditorMeshList *
scene_add_probe_reflection_plane(Scene *, ProbeReflectionPlaneDescriptor *,
                                 ProbeReflectionPlane **);

// camera
SceneEditorMeshList *scene_add_camera(Scene *, const CameraCreateDescriptor *,
                                      Camera **);

/* ===  Scene Meshes === */
SceneStatus scene_add_mesh(Scene *, Mesh *, const char *, const SceneAddFlag);
void scene_add_mesh_ref_list(Scene *, MeshRefList *, const char *,
                             const SceneAddFlag);

void scene_add_mesh_pipeline(Scene *, Mesh *, const ScenePipeline, const char *,
                             const SceneAddFlag);

void scene_add_mesh_pipeline_ref_list(Scene *, MeshRefList *,
                                      const ScenePipeline, const char *,
                                      const SceneAddFlag);

void scene_remove_mesh(Scene *, Mesh *, const ScenePipeline);
void scene_remove_mesh_ref_list(Scene *, MeshRefList *, const ScenePipeline);

#endif
