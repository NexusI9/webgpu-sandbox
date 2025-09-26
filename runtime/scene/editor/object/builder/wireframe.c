#include "wireframe.h"

#include "backend/std_pipeline/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/topology/core.h"
#include "runtime/mesh/topology/wireframe.h"
#include "runtime/pipeline/render.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/mesh/core.h"

/**
   Setup a wireframe mesh with the given vertex/index attributes and color.
   Since gizmo is part of the Fixed pipeline. It's necessary to :
   1. call mesh wireframe topology manually
   2. set override topology wireframe

   For the casual pipeline ('lit' and 'unlit' meshes), the wireframe is
   automatically handled during the scene build scene.
   However since here gizmo are part of the fixed pipeline, it's the developer
   responsibility to handle the wireframe generation as well as the overriden
   topology/shader to be rendered all the time.
 */
void seo_create_wireframe(Mesh *mesh,
                          const SEOCreateWireframeDescriptor *desc) {

  // create mesh from vertex/index attributes
  mesh_create(mesh, &(MeshCreateDescriptor){
                        .index = *desc->index,
                        .vertex = *desc->vertex,
                        .name = desc->name,
                    });

  // generate wirerfame topology
  MeshTopology base_topo = mesh_topology_base(mesh);
  MeshTopologyWireframe *wireframe_topo = &mesh->topology.wireframe;
  mesh_topology_wireframe_create(&base_topo, wireframe_topo);

  // set wireframe shader
  mesh_shader_create_fixed(mesh,
                           &(ShaderCreateDescriptor){
                               .name = "SEO wireframe shader",
                               .pipeline = std_render_pipeline(RenderPipelineType_Line),
                           });

  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Fixed), 1, 0,
                             desc->color);

  // set override topology and shader as wireframe
  mesh_topology_set_override(mesh, mesh_topology_wireframe(mesh));
}
