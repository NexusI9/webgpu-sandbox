#ifndef _RENDERER_CORE_H_
#define _RENDERER_CORE_H_

#include <emscripten/html5.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "./render_pass/render_pass.h"
#include "backend/ao_bake/ao_bake.h"
#include "backend/ao_bake/core.h"
#include "backend/clock.h"
#include "backend/compute/core.h"
#include "backend/profiler.h"
#include "backend/registry.h"
#include "backend/renderer/render_pass/visibility.h"
#include "backend/stat.h"
#include "backend/ubo.h"
#include "render_pass/core.h"
#include "runtime/pipeline/pipeline.h"
#include "runtime/pipeline/render.h"
#include "runtime/texture/core.h"
#include "runtime/texture/texture.h"
#include "webgpu/webgpu.h"

#define RENDERER_MAX_HOOK 6
#define RENDERER_DPI_AUTO 0
#define RENDERER_DRAW_MODE_COUNT 4

typedef enum {
  RendererDrawMode_Boundbox = 1 << 0,
  RendererDrawMode_Wireframe = 1 << 1,
  RendererDrawMode_Solid = 1 << 2,
  RendererDrawMode_Texture = 1 << 3,
} RendererDrawMode;

typedef enum {
  RendererPipeline_Undefined = 0,
  // Dynamic
  RendererPipeline_Dynamic_Unlit = 1 << 0,
  RendererPipeline_Dynamic_Lit = 1 << 1,
  RendererPipeline_Dynamic_LitShadow = 1 << 2,
  RendererPipeline_Dynamic_LitAlpha = 1 << 3,
  // Fixed
  RendererPipeline_Fixed_Background = 1 << 4,
  RendererPipeline_Fixed = 1 << 5,
  RendererPipeline_Fixed_Selection = 1 << 6,
  RendererPipeline_Fixed_Front = 1 << 7,
  RendererPipeline_Fixed_UI = 1 << 8,
} RendererPipeline;
#define RENDERER_PIPELINE_COUNT 9

typedef enum {
  RendererMeshPass_Default,
  RendererMeshPass_Outline,
  RendererMeshPass_Gizmo,
} RendererMeshPass;
#define RENDERER_MESH_PASS_COUNT 3


typedef enum {
  RendererMeshStates_Hidden,
} RendererMeshStates;
#define RENDERER_MESH_STATE_COUNT 1

typedef enum {
  RendererStatus_Success,
  RendererStatus_MeshVisible,
  RendererStatus_MeshHidden,
  RendererStatus_UndefError,
} RendererStatus;

typedef struct {
  cclock *clock;
  WGPUColor background;
  const double dpi;
  const int width;
  const int height;
} RendererCreateDescriptor;

typedef void (*renderer_draw_callback)(void *);

typedef struct {
  renderer_draw_callback callback;
  void *data;
} RendererDrawCallback;

typedef struct {
  RendererDrawCallback entries[RENDERER_MAX_HOOK];
  ssize_t length;
} RendererDrawCallbackList;

typedef struct Renderer {

  reg_id_t id;

  cclock clock; // update clock delta on draw
  Profiler profiler;

  // References List (ptr)
  MeshRefList pipelines[RENDERER_PIPELINE_COUNT];

  /*
    Versatile list used to store mesh pointers depending on numerous states
    (such as built, hidden...). Having such array allows to:
      - Prevent having booleans polluting the Mesh struct
      - Data-Oriented friendly approach so each Meshes with the same states can
        be easily access and given per array instructions.
      - Faster access to meshes sharing the same states.
   */
  MeshRefList mesh_state[RENDERER_MESH_STATE_COUNT];

  struct {
    double dpi;
    WGPUColor background;
    int width, height;
  } context;

  // cached texture shared throughout parent scene objects
  struct {
    RendererTextureAO ambient_occlusion;
  } texture; // TODO Make a TextureManager

  RendererDrawMode draw_mode;
  RendererDrawCallbackList callbacks[RENDERER_DRAW_MODE_COUNT];
  
  RenderPassList mesh_pass[RENDERER_DRAW_MODE_COUNT];
  RenderPassList plane_reflection_pass;
  RenderPassList probe_reflection_pass;
  RenderPassList shadow_map_pass;
  
  ComputePass compute_pass;

} Renderer;

typedef struct {
  Renderer *renderer;
} RendererRenderDescriptor;

EXTERN_C_BEGIN

void renderer_create(Renderer *, const RendererCreateDescriptor *);

void renderer_destroy(Renderer *);
void renderer_set_draw_mode(Renderer *, const RendererDrawMode);

void renderer_draw_layout_callback(void *);

void renderer_add_draw_callback(Renderer *, renderer_draw_callback, void *,
                                const int);

renderer_draw_callback renderer_find_draw_callback(Renderer *,
                                                   renderer_draw_callback);

void renderer_draw(Renderer *);

// accessors
static inline const RendererDrawMode renderer_draw_mode(Renderer *renderer) {
  return renderer->draw_mode;
}

static inline cclock *renderer_clock(Renderer *renderer) {
  return &renderer->clock;
}

static inline RenderPassList *
renderer_active_mesh_pass_list(Renderer *renderer) {
  return &renderer->mesh_pass[__builtin_ctz(renderer->draw_mode)];
}

static inline RenderPassList *
renderer_mode_mesh_pass_list(Renderer *rd, const RendererDrawMode mode) {
  return &rd->mesh_pass[__builtin_ctz(mode)];
}

static inline int renderer_width(Renderer *rd) { return rd->context.width; }

static inline int renderer_height(Renderer *rd) { return rd->context.height; }

static inline double renderer_dpi(Renderer *rd) { return rd->context.dpi; }

static inline MeshRefList *renderer_mesh_state(Renderer *rd,
                                               const RendererMeshStates state) {
  return &rd->mesh_state[state];
}

static inline MeshRefList *renderer_pipeline(Renderer *rd,
                                             const RendererPipeline pipeline) {
  return &rd->pipelines[__builtin_ctz(pipeline)];
}

#define SCENE_PIPELINE_REFLECTION_COUNT 3
static inline void renderer_reflection_pipeline_meshes(
    Renderer *rd, MeshRefList *pipelines[SCENE_PIPELINE_REFLECTION_COUNT]) {

  const RendererPipeline target_pipelines[SCENE_PIPELINE_REFLECTION_COUNT] = {
      RendererPipeline_Dynamic_Unlit,
      RendererPipeline_Dynamic_Lit,
      RendererPipeline_Dynamic_LitShadow,
  };

  for (uint8_t i = 0; i < SCENE_PIPELINE_REFLECTION_COUNT; i++)
    pipelines[i] = renderer_pipeline(rd, target_pipelines[i]);
}

#define SCENE_DYNAMIC_PIPELINE_COUNT 3
static inline void
renderer_dynamic_pipelines(Renderer *rd,
                        MeshRefList *list[SCENE_DYNAMIC_PIPELINE_COUNT],
                        size_t *count) {

  if (count)
    *count = SCENE_DYNAMIC_PIPELINE_COUNT;

  for (size_t i = 0; i < SCENE_DYNAMIC_PIPELINE_COUNT; i++)
    list[i] = renderer_pipeline(rd, (RendererPipeline)(1 << i));
}

// mutators

static inline void renderer_set_width(Renderer *rd, const int value) {
  rd->context.width = value;
}

static inline void renderer_set_height(Renderer *rd, const int value) {
  rd->context.height = value;
}

static inline void renderer_set_dpi(Renderer *rd, const double value) {
  rd->context.dpi = glm_max(1, value);
}

static inline RenderPassList *
renderer_mesh_pass_list(Renderer *renderer, const RendererDrawMode mode) {
  return &renderer->mesh_pass[__builtin_ctz(mode)];
}

// Scene related functions


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
static inline RendererStatus renderer_show_mesh(Renderer *rd, Mesh *mesh) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_enable_mesh(
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i)), mesh);

  mesh_ref_list_remove(renderer_mesh_state(rd, RendererMeshStates_Hidden),
                       mesh);

  // GLUEME
  //{
  //  scene_stat_update_draw_call_count(scene);
  //  scene_stat_update_vertex_count(scene);
  //}

  return RendererStatus_Success;
}

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
static inline RendererStatus renderer_hide_mesh(Renderer *rd, Mesh *mesh) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_disable_mesh(
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i)), mesh);

  mesh_ref_list_insert(renderer_mesh_state(rd, RendererMeshStates_Hidden),
                       mesh);

  // GLUEME
  //{
  //  scene_stat_update_draw_call_count(scene);
  //  scene_stat_update_vertex_count(scene);
  //}

  return RendererStatus_Success;
}

static inline RendererStatus renderer_show_mesh_ref_list(Renderer *rd,
                                                         MeshRefList *list) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_enable_mesh_ref_list(
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i)), list);

  // GLUEME
  //{
  //  scene_stat_update_draw_call_count(scene);
  //  scene_stat_update_vertex_count(scene);
  //}

  return RendererStatus_Success;
}

static inline RendererStatus renderer_hide_mesh_ref_list(Renderer *rd,
                                                         MeshRefList *list) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_disable_mesh_ref_list(
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i)), list);

  // GLUEME
  //{
  //  scene_stat_update_draw_call_count(scene);
  //  scene_stat_update_vertex_count(scene);
  //}

  return RendererStatus_Success;
}

static inline RendererStatus renderer_visibility_toggle_mesh(Renderer *rd,
                                                             Mesh *mesh) {

  if (mesh_ref_list_find(renderer_mesh_state(rd, RendererMeshStates_Hidden),
                         mesh, NULL)) {
    renderer_show_mesh(rd, mesh);
    return RendererStatus_MeshVisible;
  } else {
    renderer_hide_mesh(rd, mesh);
    return RendererStatus_MeshHidden;
  }
}


EXTERN_C_END

#endif
