#ifndef _RENDERER_VISIBILITY_H_
#define _RENDERER_VISIBILITY_H_

#include "core.h"

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

/**
   Show the mesh by pushing it to the pipeline ref list
 */
RendererStatus renderer_show_mesh(Renderer *, const RendererDrawMode,
                                  const RendererLayer, Mesh *);

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
RendererStatus renderer_hide_mesh(Renderer *, const RendererDrawMode,
                                  const RendererLayer, Mesh *);

RendererStatus renderer_show_mesh_ref_list(Renderer *, const RendererDrawMode,
                                           const RendererLayer, MeshRefList *);

RendererStatus renderer_hide_mesh_ref_list(Renderer *, const RendererDrawMode,
                                           const RendererLayer, MeshRefList *);

RendererStatus renderer_visibility_toggle_mesh(Renderer *,
                                               const RendererDrawMode,
                                               const RendererLayer, Mesh *);

RendererStatus renderer_show_mesh_in_pipeline(Renderer *,
                                              const RenderPipelineType, Mesh *);

RendererStatus renderer_hide_mesh_in_pipeline(Renderer *,
                                              const RenderPipelineType, Mesh *);

RendererStatus
renderer_visibility_toggle_mesh_in_pipeline(Renderer *,
                                            const RenderPipelineType, Mesh *);
#endif
