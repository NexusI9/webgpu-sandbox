#ifndef _RENDER_PASS_CORE_H_
#define _RENDER_PASS_CORE_H_
#include <webgpu/webgpu.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "runtime/mesh/mesh.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "utils/stli.h"

#define RENDER_PASS_DRAW_LAYOUT_MAX_MESH_LIST 6
#define RENDER_PASS_MAX_DRAW_LIST 12
#define RENDER_PASS_COUNT 2
#define RENDER_PASS_VIEW_CREATE -1
#define RENDER_PASS_VIEW_OVERRIDE_NONE 0
#define RENDER_PASS_VIEW_CAPACITY 256

typedef struct RenderPass RenderPass;
typedef struct RenderPassList RenderPassList;
typedef struct RenderPassDrawOptions RenderPassDrawOptions;

typedef void (*render_pass_draw_callback)(RenderPass *,
                                          const RenderPassDrawOptions *);
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
  WGPUDevice device;
  WGPUQueue queue;
  RenderPassColor color;
  RenderPassDepth depth;
  RenderPipelineMultisampleCount multisample;
  WGPUSwapChain swapchain;
  RenderPassDrawList draw_list;
  render_pass_draw_callback draw_callback;
  WGPUCommandEncoder command_encoder;
};

struct RenderPassList {
  WGPUDevice device;
  WGPUQueue queue;
  WGPUSwapChain swapchain;
  WGPUTextureView resolve_view;
  WGPUTexture resolve_texture;
  RenderPass passes[RENDER_PASS_MAX_DRAW_LIST];
  size_t length;
  render_pass_list_draw_callback draw_callback;
};

typedef struct {
  const int width;
  const int height;
  const RenderPipelineMultisampleCount multisample;
  const WGPUDevice device;
} RenderPassTextureDescriptor;

typedef struct {
  WGPUTexture texture;
  WGPUTextureView view;
  WGPULoadOp load_op;
  WGPUStoreOp store_op;
  uint32_t clear_value;
  bool read_only;
} RenderPassDepthAttachment;

typedef struct {
  WGPUTexture texture;
  WGPUTextureView view;
  WGPULoadOp load_op;
  WGPUStoreOp store_op;
  WGPUColor clear_value;
  uint32_t depth_slice;
} RenderPassColorAttachment;

typedef struct {
  const char *label;
  RenderPassColorAttachment *color;
  RenderPassDepthAttachment *depth;
  WGPUSwapChain swapchain;
  const WGPUDevice device;
  const WGPUQueue queue;
  int width;
  int height;
  RenderPipelineMultisampleCount multisample;
  const RenderPassDrawListDescriptor *draw_list;
} RenderPassCreateDescriptor;

typedef struct {
  const char *label;
  RenderPassColorAttachment *color;
  RenderPassDepthAttachment *depth;
  RenderPipelineMultisampleCount multisample;
  int width;
  int height;
  const RenderPassDrawListDescriptor *draw_list;
} RenderPassListInsert;

typedef struct {
  const char *label;
  WGPUDevice device;
  WGPUQueue queue;
  WGPUSwapChain swapchain;
  int width;
  int height;
  RenderPipelineMultisampleCount multisample;
} RenderPassListCreate;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
} RenderPassDrawDescriptor;

struct RenderPassDrawOptions {
  WGPUTextureView color, depth;
};

/* === Pass List === */
void render_pass_list_create(RenderPassList *, const RenderPassListCreate *);

void render_pass_list_insert_pass(RenderPassList *,
                                  const RenderPassListInsert *);

RenderPassStatus render_pass_list_draw_list_enable_mesh(RenderPassList *,
                                                        Mesh *,
                                                        const MeshRefList *);

RenderPassStatus render_pass_list_draw_list_disable_mesh(RenderPassList *,
                                                         Mesh *,
                                                         const MeshRefList *);

RenderPassStatus
render_pass_list_draw_list_enable_mesh_ref_list(RenderPassList *, MeshRefList *,
                                                const MeshRefList *);

RenderPassStatus render_pass_list_draw_list_disable_mesh_ref_list(
    RenderPassList *, MeshRefList *, const MeshRefList *);

RenderPassStatus render_pass_list_draw_list_enable_all(RenderPassList *);
RenderPassStatus render_pass_list_draw_list_disable_all(RenderPassList *);

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

RenderPassStatus render_pass_draw_list_enable_mesh(RenderPass *, Mesh *,
                                                   const MeshRefList *);

RenderPassStatus render_pass_draw_list_disable_mesh(RenderPass *, Mesh *,
                                                    const MeshRefList *);

RenderPassStatus
render_pass_draw_list_enable_mesh_ref_list(RenderPass *, MeshRefList *,
                                           const MeshRefList *);
RenderPassStatus
render_pass_draw_list_disable_mesh_ref_list(RenderPass *, MeshRefList *,
                                            const MeshRefList *);

RenderPassDrawLayout *
render_pass_find_draw_layout_from_mesh(RenderPassDrawList *, const Mesh *);

RenderPassStatus render_pass_draw_list_enable_all(RenderPass *);
RenderPassStatus render_pass_draw_list_disable_all(RenderPass *);

RenderPassDrawLayout *
render_pass_find_draw_layout_from_mesh_ref_list(RenderPassDrawList *,
                                                const MeshRefList *);
#endif
