#include "generator.h"

WGPUShaderModule create_shader(const state_t* state, const char *code, const char *label) {
  WGPUShaderModuleWGSLDescriptor wgsl = {
      .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor, .code = code};

  return wgpuDeviceCreateShaderModule(
      state->wgpu.device,
      &(WGPUShaderModuleDescriptor){.nextInChain = (WGPUChainedStruct *)(&wgsl),
                                    .label = label});
}

WGPUBuffer create_buffer(const state_t* state, const void *data, size_t size, WGPUBufferUsage usage) {
  // prepare buffer object
  WGPUBuffer buffer = wgpuDeviceCreateBuffer(
      state->wgpu.device,
      &(WGPUBufferDescriptor){.usage = WGPUBufferUsage_CopyDst | usage,
                              .size = size});

  // populate buffer
  wgpuQueueWriteBuffer(state->wgpu.queue, buffer, 0, data, size);
  return buffer;
}

WGPUSwapChain create_swapchain(const state_t* state) {
  WGPUSurface surface = wgpuInstanceCreateSurface(
      state->wgpu.instance,
      &(WGPUSurfaceDescriptor){
          .nextInChain = (WGPUChainedStruct *)(&(
              WGPUSurfaceDescriptorFromCanvasHTMLSelector){
              .chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
              .selector = state->context.name})});

  return wgpuDeviceCreateSwapChain(
      state->wgpu.device, surface,
      &(WGPUSwapChainDescriptor){.usage = WGPUTextureUsage_RenderAttachment,
                                 .format = WGPUTextureFormat_BGRA8Unorm,
                                 .width = state->context.width,
                                 .height = state->context.height,
                                 .presentMode = WGPUPresentMode_Fifo});
}
