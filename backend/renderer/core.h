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
#include "render_pass/core.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/pipeline.h"
#include "runtime/pipeline/render.h"
#include "runtime/texture/core.h"
#include "runtime/texture/texture.h"
#include "utils/hsht.h"
#include "webgpu/webgpu.h"

#define RENDERER_MAX_HOOK 6
#define RENDERER_DPI_AUTO 0

typedef enum {
  RendererDrawMode_None = 0,
  RendererDrawMode_Boundbox = 1 << 0,
  RendererDrawMode_Wireframe = 1 << 1,
  RendererDrawMode_Solid = 1 << 2,
  RendererDrawMode_Texture = 1 << 3,
  RendererDrawMode_All = ~0,
} RendererDrawMode;
#define RENDERER_DRAW_MODE_COUNT 4

/*
  Renderer has a list of mesh and sublist of mesh pointers that are called
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

typedef struct Renderer Renderer;

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
  WGPUColor background;
  const double dpi;
  const int width;
  const int height;
} RendererCreateDescriptor;

typedef void (*renderer_draw_callback)(Renderer *, void *);

typedef struct {
  renderer_draw_callback callback;
  void *data;
} RendererDrawCallback;

typedef struct {
  RendererDrawCallback entries[RENDERER_MAX_HOOK];
  ssize_t length;
} RendererDrawCallbackList;

struct Renderer {

  reg_id_t id;

  Profiler profiler;

  // References List (ptr)
  HashTable batches;

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
};

EXTERN_C_BEGIN

void renderer_create(Renderer *, const RendererCreateDescriptor *);

void renderer_destroy(Renderer *);
void renderer_set_draw_mode(Renderer *, const RendererDrawMode);

void renderer_draw_layout_callback(Renderer *, void *);

void renderer_add_draw_callback(Renderer *, renderer_draw_callback, void *,
                                const int);

renderer_draw_callback renderer_find_draw_callback(Renderer *,
                                                   renderer_draw_callback);

void renderer_draw(Renderer *);

// accessors
static inline const RendererDrawMode renderer_draw_mode(Renderer *renderer) {
  return renderer->draw_mode;
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

  return RendererStatus_Success;
}

static inline RendererStatus renderer_show_mesh_ref_list(Renderer *rd,
                                                         MeshRefList *list) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_enable_mesh_ref_list(
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i)), list);

  return RendererStatus_Success;
}

static inline RendererStatus renderer_hide_mesh_ref_list(Renderer *rd,
                                                         MeshRefList *list) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_disable_mesh_ref_list(
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i)), list);

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

void renderer_update_pass_texture(Renderer *, int, int,
                                  const RenderPipelineMultisampleCount,
                                  const double);

EXTERN_C_END

#endif
