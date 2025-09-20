#ifndef _SCENE_CORE_H_
#define _SCENE_CORE_H_

#include <cglm/types.h>
#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#include "./debug/debug.h"
#include "./editor/selection/gizmo/gizmo.h"
#include "./environment/environment.h"
#include "./layer.h"
#include "backend/clock.h"
#include "backend/registry.h"
#include "debug/core.h"
#include "editor/selection/gizmo/core.h"
#include "editor/ui/core.h"
#include "environment/core.h"
#include "event/core.h"
#include "renderer/core.h"
#include "runtime/camera/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/probe/probe.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/viewport/core.h"
#include "utils/vector/core.h"
#include "utils/vector/vec3_list.h"

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 1024
#define SCENE_EDITOR_OBJECT_LIST_CAPACITY_DEFAULT 128
#define SCENE_MESH_MAX_MESH_CAPACITY 64
#define SCENE_CAMERA_LIST_CAPACITY 16
#define SCENE_PIPELINE_COUNT 8
#define SCENE_PIPELINE_REFLECTION_COUNT 3

typedef uint8_t shader_bind_t;

typedef struct Scene Scene;

/**
    ▗▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖    ▗▄▄▄▖▗▄▄▄ ▗▄▄▄▖▗▄▄▄▖▗▄▖ ▗▄▄▖
   ▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌▐▌       ▐▌   ▐▌  █  █    █ ▐▌ ▐▌▐▌ ▐▌
    ▝▀▚▖▐▌   ▐▛▀▀▘▐▌ ▝▜▌▐▛▀▀▘    ▐▛▀▀▘▐▌  █  █    █ ▐▌ ▐▌▐▛▀▚▖
   ▗▄▄▞▘▝▚▄▄▖▐▙▄▄▖▐▌  ▐▌▐▙▄▄▖    ▐▙▄▄▖▐▙▄▄▀▗▄█▄▖  █ ▝▚▄▞▘▐▌ ▐▌

                 ▗▄▖ ▗▄▄▖    ▗▖▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
                ▐▌ ▐▌▐▌ ▐▌   ▐▌▐▌   ▐▌     █
                ▐▌ ▐▌▐▛▀▚▖   ▐▌▐▛▀▀▘▐▌     █
                ▝▚▄▞▘▐▙▄▞▘▗▄▄▞▘▐▙▄▄▖▝▚▄▄▖  █

 */

#define SCENE_EDITOR_OBJECT_TARGET_UNDEFINED UINT32_MAX

typedef struct SceneEditorObject SceneEditorObject;

typedef struct SceneEditorObjectMesh SceneEditorObjectMesh;

typedef struct {
  SceneEditorObjectMesh *mesh;
  SceneEditorObject *seo;
  float *offset;
} SEOTransformCallback;

typedef void (*seo_transform_axis_callback)(SEOTransformCallback *);

// Link each SEO Mesh a dedicated callback
struct SceneEditorObjectMesh {
  Mesh *mesh;
  // camera, light 'abstract' objects the meshes drives through transformation
  void *target;
  // index of object (ex in LightList or CameraList), not sure about this
  // flow...
  size_t target_list_index;
  seo_transform_axis_callback transform_callback[GIZMO_MODE_COUNT];
};

typedef struct {
  SceneEditorObjectMesh *entries;
  size_t capacity;
  size_t length;
} SceneEditorObjectMeshList;

struct SceneEditorObject {
  // parent scene pointer
  Scene *scene;
  SceneEditorObjectMeshList meshes;
  // origin mesh from which all sub meshes transformation will
  // depend
  Mesh *origin;
};

typedef struct {
  Scene *scene;
  size_t target_list_index;
  const WGPUDevice device;
  const WGPUQueue queue;
  Camera *camera;
  Viewport *viewport;
} SEOCreateDescriptor;

typedef struct {
  size_t length;
  size_t capacity;
  SceneEditorObject *entries;
} SceneEditorObjectList;

/**
    ▗▄▄▖▗▄▄▄▖▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▄▄▄▖ ▗▄▖ ▗▖  ▗▖
   ▐▌   ▐▌   ▐▌   ▐▌   ▐▌     █    █  ▐▌ ▐▌▐▛▚▖▐▌
    ▝▀▚▖▐▛▀▀▘▐▌   ▐▛▀▀▘▐▌     █    █  ▐▌ ▐▌▐▌ ▝▜▌
   ▗▄▄▞▘▐▙▄▄▖▐▙▄▄▖▐▙▄▄▖▝▚▄▄▖  █  ▗▄█▄▖▝▚▄▞▘▐▌  ▐▌

   TODO: Put selection struct within editor dir

 */

#define SCENE_SELECTION_LIST_CAPACITY 6
#define SCENE_SELECTION_TYPE_COUNT 3
#define SCENE_SELECTION_STATE_COUNT 2

typedef void *scene_selection_target_t;
typedef struct {
  scene_selection_target_t *entries;
  size_t length;
  size_t capacity;
} SceneSelectionTargetList;

typedef struct {
  MeshRefList *active_meshes;
  SceneSelectionTargetList *target_list;
  Vec3List *initial_attributes;
  vec3 *delta;
  const Axis axis;
  const GizmoMode transform_mode;
  Scene *scene;
} SceneSelectionTransform;

/* Callbacks */
typedef void (*scene_selection_transform_callback)(SceneSelectionTransform *);

typedef void (*scene_selection_highlight_callback)(MeshRefList *, void *);

typedef enum {
  SceneSelectionState_Default,
  SceneSelectionState_Selected,
} SceneSelectionState;

typedef enum {
  SceneSelectionType_Mesh,
  SceneSelectionType_MeshShadow, // update shadow map on move
  SceneSelectionType_SEO,
} SceneSelectionType;

typedef struct {
  // linked attribtutes ( mesh[i] <> targets[i] <> init_attr[i] )
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
  ▗▄▄▄▖▗▄▄▄ ▗▄▄▄▖▗▄▄▄▖▗▄▖ ▗▄▄▖
  ▐▌   ▐▌  █  █    █ ▐▌ ▐▌▐▌ ▐▌
  ▐▛▀▀▘▐▌  █  █    █ ▐▌ ▐▌▐▛▀▚▖
  ▐▙▄▄▖▐▙▄▄▀▗▄█▄▖  █ ▝▚▄▞▘▐▌ ▐▌

   TODO: Put editor struct within editor dir
 */

typedef struct {

  // selection sets
  SceneSelection selection;
  SceneEditorUI ui;
  SceneEditorObjectList seo_list; // cam/ lights  lists

  struct {
    Gizmo transform; // transform gizmo (unique)
    Mesh *grid;      // grid gizmo (unique)
  } gizmo;

} SceneEditor;

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

typedef enum {
  // Dynamic
  ScenePipeline_Dynamic_Unlit = 1 << 0,
  ScenePipeline_Dynamic_Lit = 1 << 1,
  ScenePipeline_Dynamic_LitShadow = 1 << 2,
  // Fixed
  ScenePipeline_Fixed_Background = 1 << 3,
  ScenePipeline_Fixed = 1 << 4,
  ScenePipeline_Fixed_Selection = 1 << 5,
  ScenePipeline_Fixed_Front = 1 << 6,
  ScenePipeline_Fixed_UI = 1 << 7,
} ScenePipeline;

struct Scene {

  reg_id_t id;

  // camera
  Camera *camera;
  Camera *active_camera;

  // viewport
  Viewport viewport;

  // Values lists
  MeshList meshes;
  LightList lights;
  CameraList cameras;
  ProbeReflectionGridList probes_reflection;
  ProbeReflectionPlaneList planes_reflection;

  // References List (ptr)
  MeshRefList pipelines[SCENE_PIPELINE_COUNT]; // meshes pipelines (for
                                               // render logic)
  SceneLayerSet layers; // meshes layer (for interaction logic)

  // TODO: only enable selection/gizmo related function for "Editor" mode since
  // will be never seen or used in actually "Game" mode
  SceneEditor editor;
  SceneRenderer renderer;
  SceneEnvironment environment;
  SceneDebug debug;
};

typedef struct {
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
WGPUQueue scene_queue(Scene *);
WGPUDevice scene_device(Scene *);

static inline MeshRefList *scene_pipeline(Scene *scene,
                                          const ScenePipeline pipeline) {
  // take lower bit
  return &scene->pipelines[__builtin_ctz(pipeline)];
}

static inline void scene_reflection_pipeline_meshes(
    Scene *scene, MeshRefList *pipelines[SCENE_PIPELINE_REFLECTION_COUNT]) {

  const ScenePipeline target_pipelines[SCENE_PIPELINE_REFLECTION_COUNT] = {
      ScenePipeline_Dynamic_Unlit,
      ScenePipeline_Dynamic_Lit,
      ScenePipeline_Dynamic_LitShadow,
  };

  for (uint8_t i = 0; i < SCENE_PIPELINE_REFLECTION_COUNT; i++)
    pipelines[i] = scene_pipeline(scene, target_pipelines[i]);
}

#endif
