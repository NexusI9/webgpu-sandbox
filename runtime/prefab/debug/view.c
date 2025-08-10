#include "view.h"
#include "../../../runtime/camera/camera.h"
#include "../../../runtime/viewport/viewport.h"
#include "../backend/renderer/scene/std_pipeline/std_pipeline.h"
#include "../runtime/primitive/plane.h"
#include "../utils/system.h"
#include "webgpu/webgpu.h"
#include <stddef.h>

static void debug_view_compute_position(DebugView *, vec3);

void debug_view_create(DebugView *debug_view,
                       const DebugViewCreateDescriptor *desc) {
  debug_view->length = 0;
  debug_view->capacity = VIEW_MAX_CAPACITY;
  debug_view->device = desc->device;
  debug_view->queue = desc->queue;
}

void debug_view_add(DebugView *debug_view, const ViewDescriptor *view) {

  if (debug_view->length == debug_view->capacity) {
    VERBOSE_PRINT("Debug view Currently holding max capacity, no more views "
                  "can be added\n");
    return;
  }

  // create view mesh
  Mesh *new_view = &debug_view->mesh[debug_view->length++];
  Primitive plane = primitive_plane();

  mesh_create_primitive(new_view, &(MeshCreatePrimitiveDescriptor){
                                      .primitive = &plane,
                                      .device = debug_view->device,
                                      .queue = debug_view->queue,
                                      .name = "debug view",
                                  });

  // set view texture
  mesh_shader_create_fixed(new_view,
                           &(ShaderCreateDescriptor){
                               .pipeline = std_pipeline(PipelineType_Screen),
                               .name = "Debug view billboard shader",
                               .label = "Debug view billboard shader",
                               .device = debug_view->device,
                               .queue = debug_view->queue,
                           });

  mesh_scale(new_view, (vec3){view->size[0], 1.0f, view->size[1]});

  // compute view new position
  vec3 new_position;
  debug_view_compute_position(debug_view, new_position);
  mesh_translate(new_view, new_position);

  // bind model & viewport matrix
  MeshUniform uModel = mesh_uniform_model(new_view);
  ViewportUniform uViewport = {0};
  CameraUniform uCamera = {0};

  // create mock orthographic projection
  float ortho_size = 1.5f;
  float ratio = 16.0f / 9.0f;
  glm_ortho(-ortho_size * ratio, ortho_size * ratio, -ortho_size, ortho_size,
            0.1f, 100.0f, uViewport.projection);

  // create mock camera
  glm_lookat((vec3){0.0f, 1.0f, 0.0f}, GLM_VEC3_ZERO, (vec3){0.0f, 0.0f, -1.0f},
             uCamera.view);

  ShaderBindGroupUniformEntry entries[3] = {
      {
          .binding = 0,
          .size = sizeof(MeshUniform),
          .data = &uModel,
          .offset = 0,
      },
      {
          .binding = 1,
          .size = sizeof(ViewportUniform),
          .data = &uViewport,
          .offset = 0,
      },
      {
          .binding = 2,
          .size = sizeof(CameraUniform),
          .data = &uCamera,
          .offset = 0,
      },
  };

  for (size_t i = 0; i < 3; i++) {
    ShaderBindGroupUniformEntry *entry = &entries[i];
    shader_update_uniform(mesh_shader_fixed(new_view), 0, entry->binding,
                          entry->data);
  }

  // bind texture view
  shader_update_texture_view(mesh_shader_fixed(new_view), 1, 0,
                             view->texture_view, WGPUTextureFormat_BGRA8Unorm);

  // bind sampler
  shader_update_sampler(mesh_shader_fixed(new_view), 1, 1,
                        &(WGPUSamplerDescriptor){
                            .addressModeU = WGPUAddressMode_ClampToEdge,
                            .addressModeV = WGPUAddressMode_ClampToEdge,
                            .addressModeW = WGPUAddressMode_ClampToEdge,
                            .minFilter = WGPUFilterMode_Linear,
                            .magFilter = WGPUFilterMode_Linear,
                            .compare = WGPUCompareFunction_Undefined,
                        });
}

void debug_view_compute_position(DebugView *debug_view, vec3 result) {

  float init_offset = -2.0f;
  float col = init_offset;
  float row = init_offset;

  for (size_t v = 1; v < debug_view->length; v++) {
    col += VIEW_MARGIN / 100.0f + debug_view->mesh[v].scale[0];

    // skip to new line
    if (col >= 2.0) {
      col = init_offset;
      row += VIEW_MARGIN / 100.0f + debug_view->mesh[v].scale[1];
    }
  }

  result[0] = col;
  result[2] = row;
}

size_t debug_view_length(DebugView *db_view) { return db_view->length; }
