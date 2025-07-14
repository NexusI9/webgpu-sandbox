#ifndef _SCENE_CORE_H_
#define _SCENE_CORE_H_

#include "../backend/clock.h"
#include "../backend/registry.h"
#include "../gizmo/list.h"
#include "webgpu/webgpu.h"

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 32
#define SCENE_MESH_MAX_MESH_CAPACITY 64
#define SCENE_CAMERA_LIST_CAPACITY 16

#define SCENE_SUCCESS 0
#define SCENE_MAX_CAPACITY_REACH 1
#define SCENE_ALLOC_FAILURE 2

typedef uint8_t shader_bind_t;

// due to depth test, need to write fully solid mesh first and then
// transparent meshes

/*
  Scene has a global list of mesh and sublist of mesh pointers that are called
  during certain render pass.
  Nothe that the meshes children also holds pointers to this global list, hence
  it's necessary to take care to handle them accordingly if a mesh is added or
  removed from the global list.

       Global List (Pool)        Pipeline Lists

                                 [Lit/ Physical Meshes]
                                 .----------.
                       .-------> | 0x3948ef |
                      |	         |----------|
       .----------.   |  .-----> | 0x49da39 |
       |  Mesh 1  | --' |        |----------|
       |----------|     |  .-->  | 0xed93fa |
       |  Mesh 3  | ----' |      '----------'
       |----------|       |
       |  Mesh 4  | ------'
       |----------|
       |  Mesh 5  | ------.
       |----------|       |     [Unlit/ Flat Meshes]
       |  Mesh 6  | ----. |     .----------.
       |----------|     |  '--> | 0x48daec |
       |  Mesh 7  | --. |       |----------|
       '----------'   | '-----> | 0x7423bc |
                      |         |----------|
                      '-------> | 0x3e2baf |
                                '----------'


   Render pass and Scene Mesh Lists work hand in hand.
   Meaning by pushing a mesh in a certain Scene Mesh List it will go through a
   predefined Renderer pipeline.

   The render passes are segmented in 2 global classes:
   - Dynamic: Will change depending on Render mode (wireframe/solid/textured).
   - Fixed: Is independant from Render mode.

   Currently the scene offers the following Mesh List depending on requirements:

   .----------.---------------.-----------.----------------.-------------------.
   |   Name   |  Shadow Pass  |  AO Pass  |  Fixed/Dynamic | Common use case   |
   |----------+---------------+-----------+----------------+-------------------|
   |   Lit    |       Y       |     Y     |     Dynamic    | Physical objects  |
   |----------+---------------+-----------+----------------+-------------------|
   |  UnLit   |        -      |     -     |     Dynamic    | Flat objects/ UI  |
   |----------+---------------+-----------+----------------+-------------------|
   |  Fixed   |        -      |     -     |      Fixed     | Gizmo/ Debug      |
   '----------'---------------'-----------'----------------'-------------------'

 */
typedef struct {
  MeshRefList background;
  MeshRefList lit;
  MeshRefList unlit;
  MeshRefList fixed;
} ScenePipelineList;

/*
  GIZMO LIST
  Gizmos' scene entities (meshes) are spearated from their data (primary
  struct). As a result gizmos meshes and data are gathered under the
  GizmoList struct.


      Mesh Pool
     .----------.
     |  Mesh 1  | -----------.
     |----------|            |      Gizmo List
     |  Mesh 2  | -----------|   .-------------.
     |----------|            '-> |  meshes [*] |
     |  Mesh n  |                |  length 2   |
     '----------'                |  -          |
                             .-> |  target *   |
      Gizmo Pool            |    '-------------'
     .-----------.          |
     | Camera 1  | ---------'
     |-----------|
     | Camera n  |
     '-----------'

 */

typedef struct {

  id_t id;

  // WGPU
  WGPUDevice *device;
  WGPUQueue *queue;

  // camera
  Camera *camera;
  Camera *active_camera;

  // viewport
  Viewport viewport;

  // VALUES LISTS
  MeshList meshes;    // meshes global list
  LightList lights;   // light list
  CameraList cameras; // camera list

  // REFERENCES LISTS (PTR)
  ScenePipelineList pipelines; // meshes render layers
  GizmoList gizmo;             // gizmo
  MeshRefList selection;       // selected mesh

} Scene;

typedef struct {
  cclock *clock;
  const ViewportCreateDescriptor* viewport;
  WGPUDevice *device;
  WGPUQueue *queue;
} SceneCreateDescriptor;

typedef void (*scene_draw_callback)(Scene *, WGPURenderPassEncoder *);
typedef void (*scene_build_callback)(Scene *);

void scene_create(Scene *, const SceneCreateDescriptor *);

// mesh pool
MeshList *scene_mesh_list(Scene *);

// dynamic rendering
Mesh *scene_new_mesh_lit(Scene *);
Mesh *scene_new_mesh_unlit(Scene *);
Mesh *scene_new_mesh_fixed(Scene *);
Mesh *scene_new_mesh_background(Scene *);

#endif
