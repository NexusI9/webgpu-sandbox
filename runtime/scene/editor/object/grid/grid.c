#include "grid.h"

#include <cglm/types.h>

#include "backend/std_pipeline/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/primitive/plane.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"

void seo_grid_create(Mesh *mesh, GizmoGridCreateDescriptor *gd) {

  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .name = "grid",
                                  .primitive = &plane,
                              });

  mesh_shader_create_fixed(
      mesh, &(ShaderCreateDescriptor){
                .pipeline = std_render_pipeline(RenderPipelineType_Grid),
                .name = "grid",
            });

  mesh_set_scale(mesh, (vec3){
                           gd->uniform.size,
                           gd->uniform.size,
                           gd->uniform.size,
                       });

  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Fixed), 1, 0,
                             &gd->uniform);
}
