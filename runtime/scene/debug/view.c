#include "view.h"
#include "../backend/std_pipeline/std_pipeline.h"
#include "../runtime/mesh/shader/shader.h"
#include "core.h"

static inline void scene_debug_view_compute_position(MeshRefList *, vec3);

const float height_ratio = 9.0f / 16.0f;

void scene_debug_view_create(SceneDebug *debug, const WGPUTextureView view) {

  // create view mesh
  Mesh *mesh = mesh_list_new_mesh(debug->pool);

  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &plane,
                                  .device = debug->device,
                                  .queue = debug->queue,
                                  .name = "Scene debug view",
                              });

  // set view texture
  mesh_shader_create_fixed(mesh,
                           &(ShaderCreateDescriptor){
                               .pipeline = std_pipeline(PipelineType_Screen),
                               .name = "Debug view billboard shader",
                               .label = "Debug view billboard shader",
                               .device = debug->device,
                               .queue = debug->queue,
                           });

  float ratio = debug->viewport->aspect * 9.0f / 16.0f;
  vec3 scale = {1.0f, 1.0f, ratio};
  glm_vec3_scale(scale, 0.3f, scale);
  mesh_set_scale(mesh, scale);

  // compute view new position
  vec3 new_position;

  scene_debug_view_compute_position(&debug->object_list[SceneDebugObject_View],
                                    new_position);
  mesh_set_position(mesh, new_position);

  // bind model matrix
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Fixed), 0, 0,
                        mesh_uniform(mesh));

  // bind texture view
  shader_update_texture_view(mesh_shader(mesh, MeshShader_Fixed), 1, 0, view,
                             WGPUTextureFormat_BGRA8Unorm);

  mesh_ref_list_insert(&debug->object_list[SceneDebugObject_View], mesh);
}

void scene_debug_view_compute_position(MeshRefList *views, vec3 result) {

  float init_offset = 0.8f;
  float col = -init_offset;
  float row = init_offset;

  for (size_t v = 0; v < views->length; v++) {
    col += VIEW_MARGIN / 100.0f + views->entries[v]->scale[0];

    // skip to new line
    if (col >= init_offset) {
      col = -init_offset;
      row -= VIEW_MARGIN / 100.0f + views->entries[v]->scale[1];
    }
  }

  result[0] = col;
  result[2] = row;
}
