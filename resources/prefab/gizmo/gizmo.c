#include "gizmo.h"
#include "../../primitive/plane.h"
#include "../runtime/material.h"
#include "../runtime/texture.h"

void gizmo_create_billboard(const GizmoCreateBillboardDescriptor *desc) {

  // create plane
  Primitive plane = primitive_plane();

  mesh_create_primitive(desc->mesh, &(MeshCreatePrimitiveDescriptor){
                                        .primitive = plane,
                                        .device = desc->device,
                                        .queue = desc->queue,
                                        .name = "light",
                                    });

  // set mesh position to light position
  mesh_position(desc->mesh, *desc->position);

  // scale down gizmo
  mesh_scale(desc->mesh, *desc->scale);

  // assign billboard shader
  mesh_set_shader(desc->mesh, &(ShaderCreateDescriptor){
                                  .device = desc->device,
                                  .queue = desc->queue,
                                  .label = "shader",
                                  .name = "shader",
                                  .path = SHADER_PATH_BILLBOARD,
                              });

  // set double side rendering
  pipeline_set_primitive(shader_pipeline(mesh_shader_texture(desc->mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  // bind view matrices
  material_texture_bind_views(desc->mesh, desc->camera, desc->viewport, 0);

  // TODO: create UI Atlas
  Texture light_texture;
  texture_create_from_file(&light_texture, desc->texture_path);

  // bind texture + sampler
  material_texture_add_texture(
      desc->mesh, &(ShaderCreateTextureDescriptor){
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

  material_texture_add_sampler(desc->mesh,
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
}
