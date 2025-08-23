#include "wireframe.h"
#include "../runtime/mesh/shader/shader.h"
#include "webgpu/webgpu.h"

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
                        .device = desc->device,
                        .queue = desc->queue,
                        .index = *desc->index,
                        .vertex = *desc->vertex,
                        .name = desc->name,
                    });

  // generate wirerfame topology
  MeshTopology base_topo = mesh_topology_base(mesh);
  MeshTopologyWireframe *wireframe_topo = &mesh->topology.wireframe;
  mesh_topology_wireframe_create(&base_topo, wireframe_topo, desc->device,
                                 desc->queue);

  // set wireframe shader
  mesh_shader_create_fixed(mesh,
                           &(ShaderCreateDescriptor){
                               .device = desc->device,
                               .queue = desc->queue,
                               .label = "SEO wireframe shader",
                               .name = "SEO wireframe shader",
                               .pipeline = std_pipeline(PipelineType_Line),
                           });

  shader_update_uniform(mesh_shader(mesh, MeshShader_Fixed), 0, 3, desc->color);

  // set override topology and shader as wireframe
  mesh_topology_set_override(mesh, mesh_topology_wireframe(mesh));
}
