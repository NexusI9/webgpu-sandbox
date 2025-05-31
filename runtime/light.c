#include "light.h"
#include "../resources/primitive/plane.h"
#include "../utils/system.h"
#include "./texture.h"
#include "camera.h"
#include "emscripten/emscripten.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <stdio.h>

/* =============================== SHADOW PROCESS ==============================
  Shadows use a Shadow Map approach. Meaning that they render multiple
  time the scene but under various view angles (each lights angles) to generate
  Depth Maps.
   1.Point light use a Cube Shadow Map: meaning that we will use
   our point light as a cube rendering 6 times ou scene with different angles
   2. For Spot light use Cascade Shadow Map

   To Generate the Depth Map we only require a Vertex Shader (no Fragment) as to
  only traslate the vertices under the light projection point of view

  We will then store those Depth Maps in each lights as TextureView
  and Sampler
  Once our Depth Map are stored we can finally use them in our "base" shaders

  Process Diagram:

                +----------------------+
                |        Light         |
                +----------------------+
                          |
                   Light Projection
                    (cube/cascade)
                          |
                  *****************
                  * Render pass 1 *
                  *****************
                          |
                  Generate Depth Map
                          |
                    Store Depth Map
                       Texture
                          |
                +----------------------+
                |        Mesh          |
                +----------------------+
                          |
                  Bind Depth Texture
                          |
                    Compare with
                      Fragment
                          |
                  *****************
                  * Render pass 2 *
                  *****************


  ===========================================================================

 */

void light_create_point(PointLight *light, PointLightDescriptor *desc) {

  // init to 0
  *light = (PointLight){0};

  // copy intensity (exponent)
  light->intensity = desc->intensity;

  // copy cutoff
  light->cutoff = cos(glm_rad(desc->cutoff));

  // copy inner cutoff
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));

  // copy near plane
  light->near = desc->near;

  // copy far plane
  light->far = desc->far;

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // init mesh (gizmo)
  light->mesh = NULL;
}

void light_create_spot(SpotLight *light, SpotLightDescriptor *desc) {

  // init to 0
  *light = (SpotLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy target
  glm_vec3_copy(desc->target, light->target);

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // copy cutoff and convert to cosinus radians
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));

  // copy angle
  light->angle = desc->angle;

  // init mesh (gizmo)
  light->mesh = NULL;
}

void light_create_ambient(AmbientLight *light, AmbientLightDescriptor *desc) {

  // init to 0
  *light = (AmbientLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // init mesh (gizmo)
  light->mesh = NULL;
}

void light_create_sun(SunLight *light, SunLightDescriptor *desc) {

  // init to 0
  *light = (SunLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy size
  light->size = desc->size;

  // copy position
  // For sun: normalize position and set it far away by default
  float distance = (float)LIGHT_SUN_DISTANCE;
  vec3 new_position;
  glm_vec3_norm(desc->position);
  glm_vec3_scale(desc->position, distance, new_position);
  glm_vec3_copy(new_position, light->position);

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // init mesh (gizmo)
  light->mesh = NULL;
}

/**
   Compute point view for Point light
   Point lights use 6 views, each pointing to different directions
 */
LightViews light_point_views(vec3 light_position, float near, float far) {

  LightViews new_views = (LightViews){.length = LIGHT_POINT_VIEWS};

  vec3 directions[LIGHT_POINT_VIEWS] = {
      {1.0f, 0.0f, 0.0f},  // +x (right)
      {-1.0f, 0.0f, 0.0f}, // -x (left)
      {0.0f, 1.0f, 0.0f},  // +y (top)
      {0.0f, -1.0f, 0.0f}, // -y (bottom)
      {0.0f, 0.0f, 1.0f},  // +z (front)
      {0.0f, 0.0f, -1.0f}, // -z (back)
  };

  vec3 ups[LIGHT_POINT_VIEWS] = {
      {0.0f, 1.0f, 0.0f},  // +x (right)
      {0.0f, 1.0f, 0.0f},  // -x (left)
      {0.0f, 0.0f, -1.0f}, // +y (top)
      {0.0f, 0.0f, 1.0f},  // -y (bottom)
      {0.0f, 1.0f, 0.0f},  // +z (front)
      {0.0f, 1.0f, 0.0f},  // -z (back)
  };

  mat4 projection;
  glm_perspective(glm_rad(90.0f), 1.0f, near, far, projection);

  /* Flipping projection X axis to match cubemap coordinates
     Probably has something to do with the fact that we see the cube maps
     from inside, so need to mirror its faces.
   */
  projection[0][0] *= -1.0f;

  for (int v = 0; v < new_views.length; v++) {

    vec3 direction;
    glm_vec3_add(light_position, directions[v], direction);

    mat4 view;
    glm_lookat(light_position, direction, ups[v], view);

    glm_mat4_mul(projection, view, new_views.views[v]);
  }

  return new_views;
}

/**
   Compute point view for spot light
 */
LightViews light_spot_view(vec3 light_position, vec3 light_target,
                           float angle) {

  LightViews new_views = (LightViews){.length = LIGHT_SPOT_VIEW};

  vec3 up = {0.0f, 1.0f, 0.0f};

  // adjust up if direction parallel to world up
  if (fabs(glm_vec3_dot(light_target, up)) > 0.99f)
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  mat4 projection;
  glm_perspective(glm_rad(angle), 1.0f, 0.1f, 100.0f, projection);

  for (int v = 0; v < new_views.length; v++) {
    mat4 view;
    glm_lookat(light_position, light_target, up, view);
    glm_mat4_mul(projection, view, new_views.views[v]);
  }

  return new_views;
}

/**
   Compute point view for sun light
   Sun Light work as the other way around compared to Point or Directionam Light
   For Sun are position agnostic, target is always 0,0,0, but the position
   simulates sun position by being super far away from the scene
 */
LightViews light_sun_view(vec3 light_position, float size) {

  LightViews new_views = (LightViews){.length = LIGHT_SPOT_VIEW};

  vec3 up = {0.0f, 1.0f, 0.0f};

  // adjust up if direction parallel to world up
  // if (fabs(glm_vec3_dot(light_position, up)) > 0.99f)
  // glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  mat4 ortho;
  glm_ortho(-size, size, -size, size, 0.1f, 100.0f, ortho);

  for (int v = 0; v < new_views.length; v++) {
    mat4 view;
    glm_lookat(light_position, (vec3){0.0f, 0.0f, 0.0f}, up, view);
    glm_mat4_mul(ortho, view, new_views.views[v]);
  }

  return new_views;
}

/**
   Insert Point light gizmo mesh to the list
 */
void light_point_create_mesh(PointLight *light,
                             const LightCreateMeshDescriptor *desc) {

  // create plane
  Primitive plane = primitive_plane();
  Mesh *light_mesh = mesh_list_insert(desc->list);

  mesh_create_primitive(light_mesh, &(MeshCreatePrimitiveDescriptor){
                                        .primitive = plane,
                                        .device = desc->device,
                                        .queue = desc->queue,
                                        .name = "point light",
                                    });

  // set mesh position to light position
  mesh_position(light_mesh, (vec3){
                                light->position[0],
                                light->position[1],
                                light->position[2],
                            });

  mesh_scale(light_mesh, (vec3){0.8f, 0.8f, 0.8f});

  // assign billboard shader
  mesh_set_shader(light_mesh, &(ShaderCreateDescriptor){
                                  .device = desc->device,
                                  .queue = desc->queue,
                                  .label = "point light shader",
                                  .name = "point light shader",
                                  .path = SHADER_PATH_BILLBOARD,
                              });

  // set double side rendering
  pipeline_set_primitive(shader_pipeline(mesh_shader_texture(light_mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  // bind view matrices
  material_texture_bind_views(light_mesh, desc->camera, desc->viewport, 0);

  // TODO: create UI Atlas
  Texture light_texture;
  texture_create_from_file(
      &light_texture, "./resources/assets/texture/ui/light/light-point.png");

  // bind texture + sampler
  material_texture_add_texture(
      light_mesh, &(ShaderCreateTextureDescriptor){
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

  material_texture_add_sampler(light_mesh,
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

  // cache poitner in destination
  mesh_indexed_list_insert(desc->destination, light_mesh);
}

/**
   Insert Spot light gizmo mesh to the list
 */
void light_spot_create_mesh(SpotLight *light,
                            const LightCreateMeshDescriptor *desc) {}

/**
   Insert Ambient light gizmo mesh to the list
 */
void light_ambient_create_mesh(AmbientLight *light,
                               const LightCreateMeshDescriptor *desc) {}

/**
   Insert Sun light gizmo mesh to the list
 */
void light_sun_create_mesh(SunLight *light,
                           const LightCreateMeshDescriptor *desc) {}
