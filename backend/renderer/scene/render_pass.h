#ifndef _RENDER_PASS_H_
#define _RENDER_PASS_H_
#include "../../../runtime/mesh/mesh.h"
#include <webgpu/webgpu.h>

#define SCENE_RENDERER_DRAW_LAYOUT_MAX_MESH_LIST 6
#define RENDER_PASS_COUNT 2

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
  RenderPassDrawLayout entries[SCENE_RENDERER_DRAW_LAYOUT_MAX_MESH_LIST];
  size_t length;
} RenderPassDrawLayoutList;

typedef struct {
  RenderPassDrawLayoutList entries[RENDER_PASS_COUNT];
  size_t length;
} RenderPassLayout;


// Descriptor
typedef struct {
  RenderPassType pass;
  RenderPassDrawLayout entries[SCENE_RENDERER_DRAW_LAYOUT_MAX_MESH_LIST];
  size_t length;
} RenderPassDrawLayoutListDescriptor;

typedef struct {
  RenderPassDrawLayoutListDescriptor entries[RENDER_PASS_COUNT];
  size_t length;
} RenderPassLayoutDescriptor;




typedef struct {

  const char *label;
  int width;
  int height;

  struct {
    WGPURenderPassColorAttachment attachment;
    WGPUTextureView target;
    WGPUColor clear_color;
  } color;

  struct {
    WGPURenderPassDepthStencilAttachment attachment;
    WGPUTextureView target;
  } depth;

} RenderPass;

typedef struct {
  WGPULoadOp load_op;
  WGPUStoreOp store_op;
  uint32_t clear_value;
  bool depthReadOnly;
} RenderPassDepthAttachment;

typedef struct {
  WGPULoadOp load_op;
  WGPUStoreOp store_op;
  WGPUColor clearValue;
  uint32_t depthSlice;
} RenderPassColorAttachment;

typedef struct {
  const char *label;
  int width;
  int height;
  RenderPassColorAttachment color;
  RenderPassDepthAttachment depth;
} RenderPassCreateDescriptor;

void render_pass_create_default(RenderPass *);
void render_pass_create(RenderPass *, const RenderPassCreateDescriptor *);

#endif
