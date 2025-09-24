#ifndef _SCENE_MESH_BUILD_H_
#define _SCENE_MESH_BUILD_H_
#include "core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/renderer/core.h"

/**
   Mesh Building process:

   The scene building process handles each layers respective essentials shader
   creation or binding process( view matrix...).

   Currently the renderer handles different passes such as :
   - Topology Creation
   - Shader creation
   - Shader bind views
   - Shader bind lights
   - Shader build pipeline layout

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

SceneStatus scene_build_mesh(Scene *, Mesh *, const ScenePipeline,
                             const SceneRendererDrawMode);
void scene_build_mesh_ref_list(Scene *, MeshRefList *, const ScenePipeline,
                               const SceneRendererDrawMode);
#endif
