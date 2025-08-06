#include "wireframe.h"
#include "../runtime/material/material.h"
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

  if (desc->pipeline == NULL) {
    VERBOSE_ERROR("No pipeline has been provided for SEO Wireframe.");
    return;
  }

  // set wireframe color from the vertex attributes
  vertex_attribute_set_color(desc->vertex, desc->color);

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
  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .device = desc->device,
                            .queue = desc->queue,
                            .label = "SEO wireframe shader",
                            .name = "SEO wireframe shader",
                            .pipeline = desc->pipeline,
                        });

  // set override topology and shader as wireframe
  mesh_topology_set_override(mesh, mesh_topology_wireframe(mesh));

  // set double sided
  material_texture_double_sided(mesh);
}
