#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "webgpu/webgpu.h"
#include "state.h"

WGPUSwapChain create_swapchain(const state_t*);
WGPUShaderModule create_shader(const state_t*, const char *, const char *);
WGPUBuffer create_buffer(const state_t*, const void *, size_t, WGPUBufferUsage);

#endif
