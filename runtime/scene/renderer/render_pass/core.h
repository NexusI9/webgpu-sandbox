#ifndef _RENDER_PASS_CORE_H_
#define _RENDER_PASS_CORE_H_
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#include "backend/postfx/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/mesh.h"
#include "runtime/pipeline/render.h"
#include "utils/stli.h"

#define RENDER_PASS_DRAW_LAYOUT_MAX_MESH_LIST 6
#define RENDER_PASS_MAX_DRAW_LIST 12
#define RENDER_PASS_COUNT 2
#define RENDER_PASS_VIEW_UNDEFINED 0
#define RENDER_PASS_VIEW_OVERRIDE_NONE 0
#define RENDER_PASS_VIEW_CAPACITY 256

typedef enum {
  RenderPassType_OnScreen,
  RenderPassType_OffScreen,
} RenderPassType;

typedef struct RenderPass RenderPass;
typedef struct RenderPassList RenderPassList;
typedef struct RenderPassDrawOptions RenderPassDrawOptions;

typedef void (*render_pass_draw_callback)(RenderPass *);
typedef void (*render_pass_mesh_preprocessor_callback)(const RenderPass *,
                                                       Mesh *, void *);
typedef void (*render_pass_list_draw_callback)(RenderPassList *);

typedef enum {
  RenderPassStatus_Success,
  RenderPassStatus_OutOfBoundDrawIndex,
  RenderPassStatus_LayoutUnfound,
  RenderPassStatus_DrawListUpdateError,
  RenderPassStatus_UndefError,
} RenderPassStatus;

typedef struct {
  MeshShader shader;
  mesh_get_topology_callback topology_callback;
  render_pass_mesh_preprocessor_callback mesh_preprocessor_callback;
  void *mesh_preprocessor_data;
  const MeshRefList *src_meshes;
  MeshRefList drawn_meshes;
} RenderPassDrawLayout;

typedef struct {
  MeshShader shader;
  mesh_get_topology_callback topology_callback;
  render_pass_mesh_preprocessor_callback mesh_preprocessor_callback;
  void *mesh_preprocessor_data;
  const MeshRefList *meshes;
} RenderPassDrawLayoutDescriptor;

typedef struct {
  RenderPassDrawLayout entries[RENDER_PASS_MAX_DRAW_LIST];
  size_t length;
} RenderPassDrawList;

typedef struct {
  RenderPassDrawLayoutDescriptor entries[RENDER_PASS_MAX_DRAW_LIST];
  size_t length;
} RenderPassDrawListDescriptor;

// Descriptor

typedef struct {
  WGPUTexture texture;
  WGPUTexture resolve_texture;
  WGPUTextureView resolve_view;
  WGPUTextureView views[RENDER_PASS_VIEW_CAPACITY];
  size_t views_length;
  WGPURenderPassColorAttachment attachment;
} RenderPassColor;

typedef struct {
  WGPUTexture texture;
  WGPUTextureView views[RENDER_PASS_VIEW_CAPACITY];
  size_t views_length;
  WGPURenderPassDepthStencilAttachment attachment;
} RenderPassDepth;

/**
   Basically a RenderPass is just a config with a color, depth attachment and a
   builtin encoder to render layouts from the render layout.
*/
struct RenderPass {
  const char *label;
  RenderPassType type;
  RenderPassColor color;
  RenderPassDepth depth;
  RenderPipelineMultisampleCount multisample;
  RenderPassDrawList draw_list;
  render_pass_draw_callback draw_callback;
  WGPUCommandEncoder command_encoder;
  PostFx post_fx;
};

struct RenderPassList {
  struct {

    struct {
      WGPUTexture texture;
      WGPUTextureView view;
    } color;

    struct {
      WGPUTexture texture;
      WGPUTextureView view;
    } depth;
    
  } shared;

  RenderPass passes[RENDER_PASS_MAX_DRAW_LIST];
  size_t length;
  render_pass_list_draw_callback draw_callback;
};

typedef struct {
  const int width;
  const int height;
  const RenderPipelineMultisampleCount multisample;
  const WGPUTextureFormat format;
} RenderPassTextureDescriptor;

typedef struct {
  WGPUTexture texture;
  WGPUTextureFormat format;
  WGPURenderPassDepthStencilAttachment attachment;
} RenderPassDepthAttachment;

typedef struct {
  WGPUTexture texture;
  WGPUTextureFormat format;
  WGPURenderPassColorAttachment attachment;
} RenderPassColorAttachment;

typedef struct {
  const RenderPassType type;
  const char *label;
  RenderPassColorAttachment *color;
  RenderPassDepthAttachment *depth;
  int width;
  int height;
  RenderPipelineMultisampleCount multisample;
  const RenderPassDrawListDescriptor *draw_list;
} RenderPassCreateDescriptor;

typedef struct {
  const char *label;
  int width;
  int height;
  RenderPipelineMultisampleCount multisample;
} RenderPassListCreate;

struct RenderPassDrawOptions {
  WGPUTextureView color, depth;
};

/* === Pass List === */
void render_pass_list_create(RenderPassList *, const RenderPassListCreate *);

void render_pass_list_insert_pass(RenderPassList *,
                                  const RenderPassCreateDescriptor *);

/* === Pass === */

void render_pass_draw_list_copy(const RenderPassDrawListDescriptor *,
                                RenderPassDrawList *);

void render_pass_create(RenderPass *, const RenderPassCreateDescriptor *);
RenderPassStatus render_pass_update_preprocessor_data(RenderPass *, uint8_t,
                                                      void *);

RenderPassStatus render_pass_update_all_preprocessor_data(RenderPass *, void *);

StaticListStatus render_pass_view_color_insert(RenderPass *, WGPUTextureView);
StaticListStatus render_pass_view_depth_insert(RenderPass *, WGPUTextureView);

StaticListStatus render_pass_view_color_remove(RenderPass *, WGPUTextureView);
StaticListStatus render_pass_view_depth_remove(RenderPass *, WGPUTextureView);

WGPUTextureView render_pass_view_color(RenderPass *, size_t);
WGPUTextureView render_pass_view_depth(RenderPass *, size_t);
#endif
