#include "generator.h"
#include "webgpu/webgpu.h"

WGPUShaderModule create_shader(const WGPUDevice *device, char *code,
                               const char *label) {

  WGPUShaderModuleWGSLDescriptor wgsl = {
      .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
      .code = code,
  };

  WGPUShaderModule module = wgpuDeviceCreateShaderModule(
      *device, &(WGPUShaderModuleDescriptor){
                   .nextInChain = (WGPUChainedStruct *)(&wgsl),
                   .label = label,
               });

  free(code);
  return module;
}

WGPUBuffer create_buffer(const CreateBufferDescriptor *bf) {
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
