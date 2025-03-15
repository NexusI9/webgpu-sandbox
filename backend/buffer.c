#include "buffer.h"
#include "webgpu/webgpu.h"

void buffer_create_shader(WGPUShaderModule *module, const WGPUDevice *device,
                          char *code, const char *label) {

  WGPUShaderModuleWGSLDescriptor wgsl = {
      .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
      .code = code,
  };

  *module = wgpuDeviceCreateShaderModule(
      *device, &(WGPUShaderModuleDescriptor){
                   .nextInChain = (WGPUChainedStruct *)(&wgsl),
                   .label = label,
               });

  free(code);
}

void buffer_create(WGPUBuffer *buffer, const CreateBufferDescriptor *bf) {
  // prepare buffer object
  *buffer = wgpuDeviceCreateBuffer(
      *bf->device, &(WGPUBufferDescriptor){
                       .usage = WGPUBufferUsage_CopyDst | bf->usage,
                       .size = bf->size,
                       .mappedAtCreation = bf->mappedAtCreation,
                   });

  /*
    Mapped at Creation

    Map the buffer directly to the memory
    preventing an additional GPU mapping function later
    Do not allow it for GPU only buffer or buffer that
    require regular updates.
    Good for static buffer.

    Usual buffer flow is :

    1. create buffer
    2. mapAsync (CPU access)
    3. write data
    4. unmap (make usable in GPU)

*/

  // populate buffer
  // replace manual wgpuBufferGetMappedRange() + wgpuBufferUnmap()
  wgpuQueueWriteBuffer(*bf->queue, *buffer, 0, bf->data, bf->size);
}

void buffer_create_texture(ShaderTexture *shader_texture,
                           const CreateTextureDescriptor *tx) {

  // sample + texture (ShaderTexture will be used later in the shader binding
  // process)

  // create sampler
  // TODO: eventually remove sampler from shadertexture if static arguments
  // cause we could simply use it during the binding process
  shader_texture->sampler = wgpuDeviceCreateSampler(
      *tx->device, &(WGPUSamplerDescriptor){
                       .addressModeU = WGPUAddressMode_ClampToEdge,
                       .addressModeV = WGPUAddressMode_ClampToEdge,
                       .addressModeW = WGPUAddressMode_ClampToEdge,
                       .minFilter = WGPUFilterMode_Linear,
                       .magFilter = WGPUFilterMode_Linear,
                   });

  // create GPU texture handle (used for binding as texture view argument/
  // "texture gpu reference")
  shader_texture->texture = wgpuDeviceCreateTexture(
      *tx->device,
      &(WGPUTextureDescriptor){
          .size =
              {
                  .width = tx->width,
                  .height = tx->height,
                  .depthOrArrayLayers = 1,
              },
          .format = WGPUTextureFormat_RGBA8Uint,
          .mipLevelCount = 1,
          .sampleCount = 1,
          .dimension = WGPUTextureDimension_2D,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
      });

  // create stagging buffer (CPU 2 GPU)
  WGPUBuffer stagging_buffer;
  buffer_create(&stagging_buffer, &(CreateBufferDescriptor){
                                      .device = tx->device,
                                      .queue = tx->queue,
                                      .data = tx->data,
                                      .mappedAtCreation = true,
                                      .size = tx->size,
                                  });

  // Encoder records GPU operations:
  // - Texture upload
  // - Buffer upload
  // - Render passes
  // - Compute passes
  WGPUCommandEncoder encoder =
      wgpuDeviceCreateCommandEncoder(*tx->device, NULL);

  wgpuCommandEncoderCopyBufferToTexture(
      encoder,
      // Source buffer where image data is stored in CPU memory.
      &(WGPUImageCopyBuffer){
          .buffer = stagging_buffer,
          .layout =
              {
                  .offset = 0,
                  .bytesPerRow = tx->width * 4,
                  .rowsPerImage = tx->height,
              },
      },
      // Destination texture in GPU memory.
      &(WGPUImageCopyTexture){
          .texture = shader_texture->texture,
          .mipLevel = 0,
          .origin = {0, 0, 0},
          .aspect = WGPUTextureAspect_All,
      },
      // Size (width, height, depth) of the copied region.
      &(WGPUExtent3D){tx->width, tx->height, 1});

  WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(encoder, NULL);
  wgpuQueueSubmit(*tx->queue, 1, &commandBuffer);
}
