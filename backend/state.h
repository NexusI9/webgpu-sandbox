#ifndef _STATE_H_
#define _STATE_H_

#include "webgpu/webgpu.h"

typedef struct {

  struct {
    const char *name;
    int width;
    int height;
    int dpi;
  } context;

  struct {
    WGPUInstance instance;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSwapChain swapchain;
    WGPURenderPipeline pipeline;
  } wgpu;

  struct {
    WGPUBuffer v_buffer, i_buffer, u_buffer;
    WGPUBindGroup bind_group;
  } store;

    struct{
	float rot;
    } uniform;

} state_t;


#endif
