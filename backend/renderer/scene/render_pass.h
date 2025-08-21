#ifndef _RENDER_PASS_H_
#define _RENDER_PASS_H_
#include "../../../runtime/mesh/mesh.h"
#include "../runtime/mesh/shader/shader.h"
#include <webgpu/webgpu.h>

#define RENDER_PASS_DRAW_LAYOUT_MAX_MESH_LIST 6
#define RENDER_PASS_MAX_DRAW_LIST 6
#define RENDER_PASS_COUNT 2
#define RENDER_PASS_VIEW_CREATE -1

typedef struct RenderPass RenderPass;

typedef void (*render_pass_draw_callback)(RenderPass *,
                                          WGPUTextureView, WGPUCommandEncoder);

typedef struct {
  mesh_get_shader_callback shader_callback;
  mesh_get_topology_callback topology_callback;
  MeshRefList *meshes;
} RenderPassDrawLayout;

typedef struct {
  RenderPassDrawLayout entries[RENDER_PASS_MAX_DRAW_LIST];
  size_t length;
} RenderPassDrawList;

// Descriptor

typedef struct {
  WGPURenderPassColorAttachment attachment;
} RenderPassColor;

typedef struct {
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
  PipelineMultisampleCount multisample;
  WGPUSwapChain swapchain;
  int width;
  int height;
  RenderPassDrawList draw_list;
  render_pass_draw_callback draw_callback;
  WGPURenderPassEncoder encoder;
};

typedef struct {
  WGPUDevice device;
  WGPUQueue queue;
  WGPUSwapChain swapchain;
  WGPUTextureView msaa;
  RenderPass passes[RENDER_PASS_MAX_DRAW_LIST];
  size_t length;
} RenderPassList;

typedef struct {
  const int width;
  const int height;
  const PipelineMultisampleCount multisample;
  const WGPUDevice device;
} RenderPassTextureDescriptor;

typedef struct {
  WGPUTextureView *view;
  WGPULoadOp load_op;
  WGPUStoreOp store_op;
  uint32_t clear_value;
  bool read_only;
} RenderPassDepthAttachment;

typedef struct {
  WGPUTextureView *view;
  WGPULoadOp load_op;
  WGPUStoreOp store_op;
  WGPUColor clear_value;
  uint32_t depth_slice;
} RenderPassColorAttachment;

typedef struct {
  const char *label;
  RenderPassColorAttachment color;
  RenderPassDepthAttachment depth;
  WGPUSwapChain swapchain;
  const WGPUDevice device;
  const WGPUQueue queue;
  int width;
  int height;
  PipelineMultisampleCount multisample;
  const RenderPassDrawList *draw_list;
} RenderPassCreateDescriptor;

typedef struct {
  const char *label;
  RenderPassColorAttachment color;
  RenderPassDepthAttachment depth;
  PipelineMultisampleCount multisample;
  int width;
  int height;
  const RenderPassDrawList *draw_list;
} RenderPassListInsert;

typedef struct {
  const char *label;
  WGPUDevice device;
  WGPUQueue queue;
  WGPUSwapChain swapchain;
  int width;
  int height;
  PipelineMultisampleCount multisample;
} RenderPassListCreate;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
} RenderPassDrawDescriptor;

void render_pass_set_draw_list(RenderPass *, const RenderPassDrawList *);

void render_pass_create(RenderPass *, const RenderPassCreateDescriptor *);

void render_pass_draw(RenderPass *);

void render_pass_list_draw(RenderPassList *);

void render_pass_list_create(RenderPassList *, const RenderPassListCreate *);

void render_pass_list_insert_pass(RenderPassList *,
                                  const RenderPassListInsert *);

/* Draw callbacks */
void render_pass_draw_monosample(RenderPass *, WGPUTextureView,
                                 WGPUCommandEncoder);

void render_pass_draw_multisample(RenderPass *,
                                  WGPUTextureView, WGPUCommandEncoder);

#endif
