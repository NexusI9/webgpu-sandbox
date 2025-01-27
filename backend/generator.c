#include "generator.h"
#include "webgpu/webgpu.h"

WGPUShaderModule create_shader(const WGPUDevice *device, const char *code,
                               const char *label) {
    
  WGPUShaderModuleWGSLDescriptor wgsl = {
      .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
      .code = code,
  };

  return wgpuDeviceCreateShaderModule(
      *device, &(WGPUShaderModuleDescriptor){
                              .nextInChain = (WGPUChainedStruct *)(&wgsl),
                              .label = label,
                          });
}

WGPUBuffer create_buffer(const CreateBufferDescriptor* bf) {
  // prepare buffer object
  WGPUBuffer buffer = wgpuDeviceCreateBuffer(
     *bf->device, &(WGPUBufferDescriptor){
                              .usage = WGPUBufferUsage_CopyDst | bf->usage,
                              .size = bf->size,
                          });

  // populate buffer
  wgpuQueueWriteBuffer(*bf->queue, buffer, 0, bf->data, bf->size);
  return buffer;
}

WGPUSwapChain create_swapchain(const state_t *state) {
  WGPUSurface surface = wgpuInstanceCreateSurface(
      state->wgpu.instance,
      &(WGPUSurfaceDescriptor){
          .nextInChain = (WGPUChainedStruct *)(&(
              WGPUSurfaceDescriptorFromCanvasHTMLSelector){
              .chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
              .selector = state->context.name,
          })});

  return wgpuDeviceCreateSwapChain(
      state->wgpu.device, surface,
      &(WGPUSwapChainDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment,
          .format = WGPUTextureFormat_BGRA8Unorm,
          .width = state->context.width,
          .height = state->context.height,
          .presentMode = WGPUPresentMode_Fifo,
      });
}
