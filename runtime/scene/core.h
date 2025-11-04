#ifndef _SCENE_CORE_H_
#define _SCENE_CORE_H_

#include <cglm/types.h>
#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#include "./debug/debug.h"
#include "./environment/environment.h"
#include "./layer.h"
#include "backend/clock.h"
#include "backend/postfx/core.h"
#include "backend/registry.h"
#include "backend/stat.h"
#include "backend/std_pipeline/render_shader/bloom/bloom.h"
#include "backend/std_pipeline/render_shader/composite/composite.h"
#include "backend/ubo.h"
#include "debug/core.h"
#include "environment/core.h"
#include "event/core.h"
#include "runtime/camera/core.h"
#include "runtime/scene/editor_mesh/core.h"
#include "runtime/gizmo/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/probe/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/viewport/core.h"
#include "utils/vector/core.h"
#include "utils/vector/vec3_list.h"

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 1024
#define SCENE_EDITOR_OBJECT_LIST_CAPACITY_DEFAULT 128
#define SCENE_MESH_MAX_MESH_CAPACITY 64
#define SCENE_CAMERA_LIST_CAPACITY 16

typedef uint8_t shader_bind_t;

typedef struct Scene Scene;

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

typedef enum {
  SceneStatus_Success,
  SceneStatus_MaxCapacityReach,
  SceneStatus_AllocFail,
  SceneStatus_MeshUnfound,
  SceneStatus_MeshAlreadyExists,
  SceneStatus_MeshInsertFail,
  SceneStatus_MeshAlreadyBuilt,
  SceneStatus_MeshHidden,
  SceneStatus_MeshVisible,
  SceneStatus_UnvalidPipeline,
  SceneStatus_UnsubscribedPipeline,
  SceneStatuc_UndefError,
} SceneStatus;

struct Scene {

  reg_id_t id;
  UBOManager *ubo;

  Camera *camera;
  Camera *active_camera;
  Viewport viewport;

  // Values lists
  LightList lights;
  CameraList cameras;
  ProbeList probes;
  MeshRefList meshes;
  SceneEditorMeshListArray editor_meshes;
  
  SceneLayerSet layers; // meshes layer (for interaction logic)

  SceneEnvironment environment;

  // Debug related
  Gizmo gizmo;
  SceneDebug debug;
  Statistics stats;
};

typedef struct {
  UBOManager *ubo;
  const ViewportCreateDescriptor *viewport;
} SceneCreateDescriptor;

typedef void (*scene_draw_callback)(Scene *, WGPURenderPassEncoder *);
typedef void (*scene_build_callback)(Scene *);

void scene_create(Scene *, const SceneCreateDescriptor *);
void scene_destroy(Scene *);

// scene layer quick access
MeshRefList *scene_layer_meshes(Scene *, const char *);

#endif
