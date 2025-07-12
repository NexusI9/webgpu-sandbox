#ifndef _DEBUG_VIEWS_H_
#define _DEBUG_VIEWS_H_
#include "../runtime/mesh/mesh.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>
/**
   Debug View allows to display on screen an array of textures (i.e.renders)
 */

#define VIEW_MAX_CAPACITY 12
#define VIEW_MARGIN 10

typedef struct {
  vec2 size;
  vec2 position;
  WGPUTextureView texture_view;
} ViewDescriptor;

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
} DebugViewCreateDescriptor;

typedef struct {
  Mesh mesh[VIEW_MAX_CAPACITY];
  size_t length;
  size_t capacity;
  WGPUDevice *device;
  WGPUQueue *queue;
} DebugView;

void debug_view_create(DebugView *, const DebugViewCreateDescriptor *);
void debug_view_add(DebugView *, const ViewDescriptor *);
size_t debug_view_length(DebugView *);

#endif
