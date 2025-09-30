#ifndef _SCENE_SHOW_H_
#define _SCENE_SHOW_H_

#include "./core.h"
#include "backend/logger.h"
#include "renderer/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/renderer/render_pass/visibility.h"

/**

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

#ifdef __cplusplus
extern "C" {
#endif

/**
   Show the mesh by pushing it to the pipeline ref list
 */
SceneStatus scene_show_mesh(Scene *scene, Mesh *mesh) {

  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_enable_mesh(
        scene_renderer_pass_list(&scene->renderer,
                                 (SceneRendererDrawMode)(1 << i)),
        mesh);

  mesh_ref_list_remove(scene_mesh_state(scene, SceneMeshStates_Hidden), mesh);

  return SceneStatus_Success;
}

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
SceneStatus scene_hide_mesh(Scene *scene, Mesh *mesh) {

  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_disable_mesh(
        scene_renderer_pass_list(&scene->renderer,
                                 (SceneRendererDrawMode)(1 << i)),
        mesh);

  mesh_ref_list_insert(scene_mesh_state(scene, SceneMeshStates_Hidden), mesh);

  return SceneStatus_Success;
}

SceneStatus scene_show_mesh_ref_list(Scene *scene, MeshRefList *list) {

  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_enable_mesh_ref_list(
        scene_renderer_pass_list(&scene->renderer,
                                 (SceneRendererDrawMode)(1 << i)),
        list);

  return SceneStatus_Success;
}

SceneStatus scene_hide_mesh_ref_list(Scene *scene, MeshRefList *list) {

  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_disable_mesh_ref_list(
        scene_renderer_pass_list(&scene->renderer,
                                 (SceneRendererDrawMode)(1 << i)),
        list);

  return SceneStatus_Success;
}

SceneStatus scene_visibility_toggle_mesh(Scene *scene, Mesh *mesh) {

  if (mesh_ref_list_find(scene_mesh_state(scene, SceneMeshStates_Hidden), mesh,
                         NULL))
    scene_show_mesh(scene, mesh);
  else
    scene_hide_mesh(scene, mesh);

  return SceneStatus_Success;
}

#ifdef __cplusplus
}
#endif

#endif
