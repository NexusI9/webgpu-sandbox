#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "backend/resource_manager.h"
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
  const char *html_target;
  int width, height;
  double dpi;
} Context;

typedef struct {
  // target surface
  const char *html_target;
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
static inline void context_set_width(int value) { g_context.width = value; }
static inline void context_set_height(int value) { g_context.height = value; }

// error callbacks
void context_handle_device_lost(WGPUDeviceLostReason, const char *, void *);
void context_handle_device_error(WGPUErrorType, const char *, void *);
void context_error_callback(WGPUErrorType, const char *, void *);
void context_handle_uncaptured_error(WGPUErrorType, char const *, void*);

EXTERN_C_END

#endif
