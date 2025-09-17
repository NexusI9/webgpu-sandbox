#include "utils.h"

#include <cglm/vec3.h>
#include <stddef.h>

#include "resources/loader/loader.mbin.h"
#include "./core.h"
#include "webgpu/webgpu.h"
#include "backend/std_pipeline/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "utils/color.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/primitive/core.h"

/**
   Create a gizmo transform mesh with the solid pipeline and the other relative
   pipeline settings (no depth write).

   Used to generate each gizmo handles.
 */
void gizmo_create_mesh(Mesh *mesh, Primitive *primitive,
                                 const color *rgba, const WGPUQueue queue,
                                 const WGPUDevice device) {

  // init mesh
  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = primitive,
                                  .device = device,
                                  .queue = queue,
                                  .name = "Gizmo",
                              });


  // add shader
  mesh_shader_create_fixed(mesh,
                           &(ShaderCreateDescriptor){
                               .pipeline = std_render_pipeline(RenderPipelineType_Unlit),
                               .device = device,
                               .queue = queue,
                               .label = "Gizmo shader",
                               .name = "Gizmo shader",
                           });

  // add color uniform
  const float fixed_size = GIZMO_SIZE;
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Fixed), 1, 0, (void *)rgba);
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Fixed), 1, 1, (void *)&fixed_size);
 

  // scale gizmo (cpu side as well, so the hitbox are correct dimension)
  // mesh_set_scale(mesh, (vec3){gizmo_size, gizmo_size, gizmo_size});
}

/**
   Load the transform gizmom meshbinary and automate the shader/ color and angle
   process.
 */
void gizmo_create_handles(
    MeshRefList *visual_list, MeshRefList *interactive_list,
    const GizmoCreateMeshDescriptor *desc) {

  // init gizmo reference list
  const size_t gizmo_mesh_count = 3;

  if (interactive_list->capacity == 0)
    mesh_ref_list_create(interactive_list, gizmo_mesh_count);

  if (visual_list->capacity == 0)
    mesh_ref_list_create(visual_list, gizmo_mesh_count);

  // load arrow mesh binary
  Primitive mesh_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = desc->mbin_path,
      .primitive = &mesh_primitive,
  });

  // create new mesh in mesh ref list in order: x, y ,z
  for (size_t i = 0; i < gizmo_mesh_count; i++) {
    Mesh *mesh = mesh_list_new_mesh(desc->list);

    gizmo_create_mesh(mesh, &mesh_primitive, gizmo_handle_color[i], desc->queue,
                                desc->device);


    // rotate
    mesh_set_rotation(mesh, (vec3){
                          (i == 2) * 90.0f,
                          0.0f,
                          (i == 0) * -90.0f,
                      });

    // update gizmo ref list
    mesh_ref_list_insert(visual_list, mesh);
    // add to interactive list
    mesh_ref_list_insert(interactive_list, mesh);
  }

  primitive_destroy(&mesh_primitive);
}

void gizmo_origin(Gizmo *gizmo, vec3 *position) {
  glm_vec3_copy(gizmo->handles[gizmo->mode].entries[0]->position, *position);
}
