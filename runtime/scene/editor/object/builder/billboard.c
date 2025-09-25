#include "billboard.h"

#include <stdint.h>
#include <stdbool.h>

#include "runtime/primitive/plane.h"
#include "webgpu/webgpu.h"
#include "backend/std_pipeline/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "runtime/texture/create.h"
#include "runtime/mesh/core.h"

/**
   Create a plane mesh with a billboard shader
 */
void seo_create_billboard(Mesh *mesh,
                          const SEOCreateBillboardDescriptor *desc) {

  // create plane
  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &plane,
                                  .name = "SEO Billboard",
                              });

  // assign billboard shader
  mesh_shader_create_fixed(mesh,
                           &(ShaderCreateDescriptor){
                               .label = "SEO billboard shader",
                               .name = "SEO billboard shader",
                               .pipeline = std_render_pipeline(RenderPipelineType_Billboard),
                           });

  // set mesh position to light position
  mesh_set_position(mesh, *desc->position);

  // scale down gizmo
  mesh_set_scale(mesh, *desc->scale);

  // TODO: create UI Atlas
  Texture light_texture;
  texture_create_from_file(&light_texture,
                           &(TextureCreateFileDescriptor){
                               .width = TextureResolution_Undefined,
                               .height = TextureResolution_Undefined,
                               .channels = TextureChannel_Undefined,
                               .flip = true,
                               .path = desc->texture_path,
                           });

  // bind texture + sampler
  shader_update_texture(mesh_shader(mesh, MeshShader_Fixed), 1, 0,
                        &(ShaderUpdateTexture){
                            .width = light_texture.width,
                            .height = light_texture.height,
                            .data = light_texture.data,
                            .size = light_texture.size,
                            .channels = light_texture.channels,
                            .dimension = WGPUTextureViewDimension_2D,
                            .format = TEXTURE_FORMAT_OFFSCREEN_DEFAULT,
                        });

  shader_update_sampler(mesh_shader(mesh, MeshShader_Fixed), 1, 1,
                        &(WGPUSamplerDescriptor){
                            .addressModeU = WGPUAddressMode_Repeat,
                            .addressModeV = WGPUAddressMode_Repeat,
                            .addressModeW = WGPUAddressMode_Repeat,
                            .minFilter = WGPUFilterMode_Linear,
                            .magFilter = WGPUFilterMode_Linear,
                            .compare = WGPUCompareFunction_Undefined,
                        });

  const uint32_t size = 0;
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Fixed), 1, 2, (void *)&size);
}
