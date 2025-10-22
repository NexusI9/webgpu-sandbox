#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "runtime/input/core.h"
#include "runtime/pipeline/render.h"
#include <webgpu/webgpu.h>

typedef enum {
  ContextStatus_Success,
  ContextStatus_WGPUInitError,
  ContextStatus_UndefError,
} ContextStatus;

typedef struct {
  WGPUInstance instance;
  WGPUDevice device;
  WGPUQueue queue;
  WGPUAdapter adapter;
  WGPUAdapterInfo adapter_info;
  WGPUSwapChain swapchain;
  RenderPipelineMultisampleCount multisample;
  const char *html_target;
  int width, height;
  double dpi;
} Context;

typedef struct {
  const char *html_target;

  struct {
    const RenderPipelineMultisampleCount multisample_count;
  } render;

  const InputDescriptor *input;

} ContextDescriptor;

extern Context g_context;

EXTERN_C_BEGIN

ContextStatus context_init(const ContextDescriptor *);
ContextStatus context_close();

static inline WGPUDevice context_device() { return g_context.device; }
static inline WGPUQueue context_queue() { return g_context.queue; }
static inline WGPUSwapChain context_swapchain() { return g_context.swapchain; }
static inline int context_width() { return g_context.width; }
static inline int context_height() { return g_context.height; }
static inline const char *context_target() { return g_context.html_target; }
static inline double context_dpi() { return g_context.dpi; }
static inline RenderPipelineMultisampleCount context_multisample() {
  return g_context.multisample;
}

static inline void context_set_width(int value) { g_context.width = value; }
static inline void context_set_height(int value) { g_context.height = value; }
static inline void
context_set_multisample(RenderPipelineMultisampleCount value) {
  g_context.multisample = value;
}

EXTERN_C_END

#endif
