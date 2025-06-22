#include "shader.h"
#include "../backend/buffer.h"
#include "../backend/shadow_pass.h"
#include "../material/material.h"
#include "../runtime/geometry/edge/edge.h"
#include "../runtime/geometry/line/line.h"
#include "../utils/math.h"
#include "topology/wireframe.h"

/**
   Return mesh default shader
 */
Shader *mesh_shader_texture(Mesh *mesh) { return &mesh->shader.texture; }

/**
   Return mesh shadow shader
 */
Shader *mesh_shader_shadow(Mesh *mesh) { return &mesh->shader.shadow; }

/**
   Return mesh wireframe shader
 */
Shader *mesh_shader_wireframe(Mesh *mesh) { return &mesh->shader.wireframe; }

/**
   Return mesh solid shader
 */
Shader *mesh_shader_solid(Mesh *mesh) { return &mesh->shader.solid; }

/**
   Return mesh override shader
   Primarily used for fixed layer during the scene build/draw process.
 */
Shader *mesh_shader_override(Mesh *mesh) { return mesh->shader.override; }

/**
   Init mesh shadow shader.
   By default all mesh have a shadow shader to generate shadow map
   during the bind light process we will generate the depth map since that's
   where we get out scene lights.

   The init shadow shader doesn't belong to the material API as it is a
   necessary component set by default on mesh creation.
 */
void mesh_create_shadow_shader(Mesh *mesh) {

  // import shadow shader
  Shader *shadow_shader = mesh_shader_shadow(mesh);
  shader_create(shadow_shader,
                &(ShaderCreateDescriptor){
                    .path = "./runtime/assets/shader/shader.shadow.wgsl",
                    .label = "Mesh shadow shader",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "Mesh shadow shader",
                });

  // edit shader pipeline (vertex only)
  pipeline_set_stencil(shader_pipeline(shadow_shader),
                       (WGPUDepthStencilState){
                           .format = SHADOW_DEPTH_FORMAT,
                           .depthWriteEnabled = true,
                           .depthCompare = WGPUCompareFunction_Less,
                       });

  /* need to set the cullback to FRONT for point light because the light POV
   * render is flipped on the X axis to match the cubemap coordinates, such
   * negative scaling lead to set the cullback to front.*/
  pipeline_set_primitive(shader_pipeline(shadow_shader),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_Front,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });
}

/**
   Initialize Wireframe shader.
   Wireframe use a second vertex and index buffer (buffer.wireframe), since
   wireframe require to draw lines for each edges, however lines are basically
   rendered as very thin quads, which requires to duplicate each vertex once.

   The init wireframe shader doesn't belong to the material API as it is a
   necessary component set by default on mesh creation.

   Overall process:
     1. Isolate unique edges
     2. Create lines for each pair
     3. Upload data to GPU buffer
     4. Create wireframe shader
 */
void mesh_create_wireframe_shader(Mesh *mesh) {

  Shader *wireframe_shader = mesh_shader_wireframe(mesh);

  if (wireframe_shader->name)
    shader_destroy(wireframe_shader);

  // create shader
  shader_create(wireframe_shader,
                &(ShaderCreateDescriptor){
                    .path = "./runtime/assets/shader/shader.line.wgsl",
                    .label = "Mesh wireframe shader",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "Mesh wireframe shader",
                });

  // update pipeline for double-sided
  material_texture_double_sided(mesh);

}

/**
   Initialize solid shader
 */
void mesh_create_solid_shader(Mesh *mesh) {}

/**
   Override shader allow to direct toward another shader for any rendering type.
   This can become handy for gizmo if they need to appear as "wireframe" instead
   of solid. Shader Override often comes hand in hand with Topology Override.
   Override basically means:
   "I want you to use this topology and shader no matter the rendering mode"
   (wireframe/ solid/ textured..)
 */
void mesh_shader_set_override(Mesh *mesh, Shader *shader) {
  mesh->shader.override = shader;

  Shader *solid_shader = mesh_shader_solid(mesh);

  // create shader
  shader_create(solid_shader, &(ShaderCreateDescriptor){
                                  .path = SHADER_PATH_SOLID,
                                  .label = "Mesh solid shader",
                                  .device = mesh->device,
                                  .queue = mesh->queue,
                                  .name = "Mesh solid shader",
                              });
}
