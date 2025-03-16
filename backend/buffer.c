#include "buffer.h"
#include "../runtime/texture.h"
#include "webgpu/webgpu.h"
#include "stb/stb_image.h"

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
  *buffer = wgpuDeviceCreateBuffer(*bf->device,
                                   &(WGPUBufferDescriptor){
                                       .usage = bf->usage,
                                       .size = bf->size,
                                       .mappedAtCreation = bf->mappedAtCreation,
                                   });

  /*NOTE:
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

void buffer_create_texture(WGPUTextureView *texture_view,
                           const CreateTextureDescriptor *tx) {

  // sample + texture (ShaderTexture will be used later in the shader binding
  // process)

  // TODO: Implement a chunk based upload for textures > 1024

  // create GPU texture handle (used for binding as texture view argument/
  // "texture gpu reference")
  WGPUTexture texture = wgpuDeviceCreateTexture(
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


  wgpuQueueWriteTexture(*tx->queue,
                        &(WGPUImageCopyTexture){
                            .texture = texture,
                            .mipLevel = 0,
                            .origin = {0, 0, 0},
                            .aspect = WGPUTextureAspect_All,
                        },
                        tx->data, tx->size,
                        &(WGPUTextureDataLayout){
                            .offset = 0,
                            .bytesPerRow = tx->width * TEXTURE_DEFAULT_CHANNELS,
                            .rowsPerImage = tx->height,
                        },
                        &(WGPUExtent3D){tx->width, tx->height, 1});

  stbi_image_free(tx->data);

  // create texture view (used in binding process)
  *texture_view = wgpuTextureCreateView(texture, NULL);
}
