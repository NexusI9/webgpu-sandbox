#ifndef _RENDER_PASS_H_
#define _RENDER_PASS_H_
#include "../../../runtime/mesh/mesh.h"
#include "../runtime/mesh/shader/shader.h"
#include <webgpu/webgpu.h>

#define SCENE_RENDERER_DRAW_LAYOUT_MAX_MESH_LIST 6
#define RENDER_PASS_COUNT 2
#define RENDER_PASS_VIEW_CREATE -1

typedef struct RenderPass RenderPass;

typedef enum {
  RenderPassType_Scene,
  RenderPassType_Gizmo,
} RenderPassType;

typedef struct {
  mesh_get_shader_callback shader_callback;
  mesh_get_topology_callback topology_callback;
  MeshRefList *meshes;
} RenderPassDrawLayout;

typedef struct {
  RenderPassType pass;
  RenderPassDrawLayout entries[SCENE_RENDERER_DRAW_LAYOUT_MAX_MESH_LIST];
  size_t length;
} RenderPassDrawList;

typedef struct {
  RenderPassDrawList entries[RENDER_PASS_COUNT];
  size_t length;
} RenderPassLayout;

// Descriptor

typedef struct {
  WGPURenderPassColorAttachment attachment;
} RenderPassColor;

typedef struct {
  WGPURenderPassDepthStencilAttachment attachment;
} RenderPassDepth;

struct RenderPass {
  const char *label;
  RenderPassColor color;
  RenderPassDepth depth;
  WGPURenderPassEncoder encoder;
};

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
} RenderPassCreateDescriptor;

typedef struct {
  RenderPassLayout *pass_layout;
  WGPUSwapChain *swapchain;
  PipelineMultisampleCount multisample;
  WGPUTextureView *msaa_view;
  const WGPUDevice device;
  const WGPUQueue queue;
  RenderPass *pass_list;
} RenderPassDrawDescriptor;

typedef void (*render_pass_color_attachment_callback)(RenderPass *);
typedef void (*render_pass_draw_callback)(RenderPass *, RenderPassLayout *,
                                          WGPUTextureView *, WGPUTextureView *,
                                          WGPUCommandEncoder *);

void render_pass_create(RenderPass *, const RenderPassCreateDescriptor *);

void render_pass_draw(RenderPassDrawDescriptor *);

#endif
