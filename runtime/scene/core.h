#ifndef _SCENE_CORE_H_
#define _SCENE_CORE_H_

#include "../backend/clock.h"
#include "../backend/registry.h"
#include "../backend/renderer/renderer.h"
#include "./editor/gizmo/gizmo.h"
#include "./editor/object/object.h"
#include "./layer.h"
#include "editor/gizmo/core.h"
#include "editor/object/core.h"
#include "webgpu/webgpu.h"
#include <stddef.h>

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 32
#define SCENE_MESH_MAX_MESH_CAPACITY 64
#define SCENE_CAMERA_LIST_CAPACITY 16
#define SCENE_PIPELINE_COUNT 7

#define SCENE_SELECTION_LIST_CAPACITY 6
#define SCENE_SELECTION_TYPE_COUNT 2
#define SCENE_SELECTION_STATE_COUNT 2
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

typedef enum {
  SceneStatus_Success,
  SceneStatus_MaxCapacityReach,
  SceneStatus_AllocFail,
  SceneStatus_MeshUnfound,
  SceneStatus_MeshAlreadyExists,
  SceneStatus_MeshInsertFail,
  SceneStatuc_UndefError,
} SceneStatus;

typedef uint8_t shader_bind_t;

typedef struct Scene Scene;

typedef enum {
  // Dynamic
  ScenePipeline_Dynamic_Background,
  ScenePipeline_Dynamic_Lit,
  ScenePipeline_Dynamic_Unlit,
  // Fixed
  ScenePipeline_Fixed,
  ScenePipeline_Fixed_Selection,
  ScenePipeline_Fixed_Front,
  ScenePipeline_Fixed_UI,
} ScenePipeline;

/**


   ===== SELECTION =====


 */

typedef void *scene_selection_target_t;
typedef struct {
  scene_selection_target_t *entries;
  size_t length;
  size_t capacity;
} SceneSelectionTargetList;

typedef void (*scene_selection_transform_callback)(MeshRefList *,
                                                   SceneSelectionTargetList *,
                                                   Vec3List *, vec3, const Axis,
                                                   const GizmoTransformMode,
                                                   void *);

typedef void (*scene_selection_highlight_callback)(MeshRefList *, void *);

typedef enum {
  SceneSelectionState_Default,
  SceneSelectionState_Selected,
} SceneSelectionState;

typedef enum {
  SceneSelectionType_Mesh,
  SceneSelectionType_SEO,
} SceneSelectionType;

typedef struct {

  MeshRefList meshes[SCENE_SELECTION_STATE_COUNT];
  SceneSelectionTargetList targets[SCENE_SELECTION_STATE_COUNT];
  Vec3List initial_attributes;

  scene_selection_highlight_callback highlight_callback;
  void *highlight_data;

  scene_selection_transform_callback transform_callback;
  void *transform_data;

} SceneSelectionFilter;

typedef struct {
  SceneSelectionFilter filters[SCENE_SELECTION_TYPE_COUNT];
} SceneSelection;

/**


   ===== EDITOR =====


 */

typedef struct {

  // selection sets
  SceneSelection selection;

  SceneEditorObjectList seo_list; // cam/ lights  lists

  struct {
    GizmoTransform transform; // transform gizmo (unique)
    Mesh *grid;               // grid gizmo (unique)
  } gizmo;

} SceneEditor;

struct Scene {

  id_t id;

  // camera
  Camera *camera;
  Camera *active_camera;

  // viewport
  Viewport viewport;

  // Values lists
  MeshList meshes;    // meshes pool
  LightList lights;   // light list
  CameraList cameras; // camera list

  // References List (ptr)
  MeshRefList
      pipelines[SCENE_PIPELINE_COUNT]; // meshes pipelines (for render logic)
  SceneLayerSet layers;                // meshes layer (for interaction logic)

  // TODO: only enable selection/gizmo related function for "Editor" mode since
  // will be never seen or used in actually "Game" mode
  SceneEditor editor;
  SceneRenderer renderer;
};

typedef struct {
  cclock *clock;
  const ViewportCreateDescriptor *viewport;
  const SceneRendererCreateDescriptor *renderer;
} SceneCreateDescriptor;

typedef void (*scene_draw_callback)(Scene *, WGPURenderPassEncoder *);
typedef void (*scene_build_callback)(Scene *);

void scene_create(Scene *, const SceneCreateDescriptor *);

// mesh pool
MeshList *scene_mesh_list(Scene *);

// scene layer quick access
MeshRefList *scene_layer_meshes(Scene *, const char *);
WGPUQueue *scene_queue(Scene *);
WGPUDevice *scene_device(Scene *);

#endif
