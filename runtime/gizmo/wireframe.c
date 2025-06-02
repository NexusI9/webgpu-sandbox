#include "wireframe.h"
#include "../material/material.h"

/**
   Setup a wireframe mesh with the given vertex/index attributes and color
 */
void gizmo_create_wireframe(Mesh *mesh,
                            const GizmoCreateWireframeDescriptor *desc) {

  // set wireframe color from the vertex attributes
  // vertex_attribute_set_color(desc->vertex, desc->color);
  // vertex_attribute_set_uv(desc->vertex, &(vec2){desc->thickness,
  // desc->side});

  // create mesh from vertex/index attributes
  mesh_create(mesh, &(MeshCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .index = *desc->index,
                        .vertex = *desc->vertex,
                        .name = desc->name,
                    });

  /* copy mesh's wireframe vertex attribute, index and wireframe shader into the
  "main" (texture) vertex attribute, index + shader

     Mesh create
          '- Init Shadow shader
          __     '- Init Wireframe shader
      .--|	          '- Create Wireframe dedicated vertex attribute
     |   '--		  '- Wireframe dedicated Index
     |
     |
     '---> Override mesh base vertex/index list + texture shader w/ wireframe


  */

  // transfert wireframe vertex attribute to base vertex
  vertex_attribute_copy(&mesh->vertex.wireframe.attribute,
                        &mesh->vertex.base.attribute);

  // transfert wireframe vertex index to base index
  vertex_index_copy(&mesh->vertex.wireframe.index, &mesh->vertex.base.index);

  // set wireframe shader
  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .device = desc->device,
                            .queue = desc->queue,
                            .label = "Gizmo wireframe shader",
                            .name = "Gizmo wireframe shader",
                            .path = SHADER_PATH_LINE,
                        });

  // set double sided
  material_texture_double_sided(mesh);

  // bind views
  material_texture_bind_views(mesh, desc->camera, desc->viewport, 0);
}
