#include "billboard.h"
#include "../primitive/plane.h"
#include "../runtime/material/material.h"
#include "../runtime/texture/texture.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

/**
   Create a plane mesh with a billboard shader
 */
void gizmo_create_billboard(Mesh *mesh,
                            const GizmoCreateBillboardDescriptor *desc) {

  // create plane
  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = plane,
                                  .device = desc->device,
                                  .queue = desc->queue,
                                  .name = "Gizmo Billboard",
                              });

  // set mesh position to light position
  mesh_translate(mesh, *desc->position);

  // scale down gizmo
  mesh_scale(mesh, *desc->scale);

  // assign billboard shader
  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .device = desc->device,
                            .queue = desc->queue,
                            .label = "Gizmo billboard shader",
                            .name = "Gizmo billboard shader",
                            .path = SHADER_PATH_BILLBOARD,
                        });

  // set double side rendering
  material_texture_double_sided(mesh);

  // TODO: create UI Atlas
  Texture light_texture;
  texture_create_from_file(&light_texture, desc->texture_path, true);

  // bind texture + sampler
  material_texture_add_texture(
      mesh, &(ShaderCreateTextureDescriptor){
                .group_index = 1,
                .entry_count = 1,
                .visibility = WGPUShaderStage_Fragment,
                .entries = (ShaderBindGroupTextureEntry[]){{
                    .binding = 0,
                    .width = light_texture.width,
                    .height = light_texture.height,
                    .data = light_texture.data,
                    .size = light_texture.size,
                    .channels = light_texture.channels,
                    .dimension = WGPUTextureViewDimension_2D,
                    .format = WGPUTextureFormat_RGBA8Unorm,
                    .sample_type = WGPUTextureSampleType_Float,
                }},
            });

  material_texture_add_sampler(mesh,
                               &(ShaderCreateSamplerDescriptor){
                                   .group_index = 1,
                                   .entry_count = 1,
                                   .visibility = WGPUShaderStage_Fragment,
                                   .entries = (ShaderBindGroupSamplerEntry[]){{
                                       .binding = 1,
                                       .addressModeU = WGPUAddressMode_Repeat,
                                       .addressModeV = WGPUAddressMode_Repeat,
                                       .addressModeW = WGPUAddressMode_Repeat,
                                       .minFilter = WGPUFilterMode_Linear,
                                       .magFilter = WGPUFilterMode_Linear,
                                       .type = WGPUSamplerBindingType_Filtering,
                                       .compare = WGPUCompareFunction_Undefined,
                                   }},
                               });

  const uint32_t size = 0;
  material_texture_add_uniform(
      mesh, &(ShaderCreateUniformDescriptor){
                .group_index = 1,
                .entry_count = 1,
                .visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex,
                .entries = (ShaderBindGroupUniformEntry[]){{
                    .binding = 2,
                    .data = (void *)&size,
                    .size = sizeof(uint32_t),
                    .offset = 0,
                }},
            });
}
