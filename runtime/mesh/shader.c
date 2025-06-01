#include "shader.h"
#include "../resources/geometry/edge.h"
#include "../resources/prefab/debug/line.h"
#include "../backend/shadow_pass.h"
#include "../backend/buffer.h"
#include "../utils/math.h"
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
Shader *mesh_shader_solid(Mesh *mesh) { return &mesh->shader.texture; }


/**
   Init mesh shadow shader.
   By default all mesh have a shadow shader to generate shadow map
   during the bind light process we will generate the depth map since that's
   where we get out scene lights.

   The init shadow shader doesn't belong to the material API as it is a
   necessary component set by default on mesh creation.
 */
void mesh_init_shadow_shader(Mesh *mesh) {

  // import shadow shader
  Shader *shadow_shader = mesh_shader_shadow(mesh);
  shader_create(shadow_shader,
                &(ShaderCreateDescriptor){
                    .path = "./runtime/assets/shader/shader.shadow.wgsl",
                    .label = "shadow",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "shadow",
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
void mesh_init_wireframe_shader(Mesh *mesh) {

  Shader *wireframe_shader = mesh_shader_wireframe(mesh);
  MeshVertex *wireframe_vertex = &mesh->vertex.wireframe;
  WGPUBuffer vertex_buffer = wireframe_vertex->attribute.buffer;
  WGPUBuffer index_buffer = wireframe_vertex->index.buffer;

  // reset existing wireframe buffer if exists
  if (vertex_buffer) {
    wgpuBufferRelease(vertex_buffer);
    vertex_buffer = NULL;
  }

  if (index_buffer) {
    wgpuBufferRelease(index_buffer);
    index_buffer = NULL;
  }

  // TODO: replace with opaque pointer implementation (shader == null)
  if (wireframe_shader->name)
    shader_destroy(wireframe_shader);

  /*
    Create a edge hash set to store unique edges
    To store unique data, hash tables are more efficient since we can directly
    check if the hash exist via the hashing function instead of every time
    traverse the array and compate the data.

      Object                                  Hash Table
    .--------.                              .-------------.
    | attr a | --.   .-------------.        |     ..      |
    |--------|   '-- |   Hashing   | --.    |-------------|
    | attr b | --'   '-------------'   |    |     ..      |
    '--------'                         |    |-------------|
                                       '--> |     34      |
                                            |-------------|
                                            |     ..      |
                                            '-------------'

               Data Attributes + Hash = Index

   */

  EdgeHashSet edges;
  edge_hash_set_create(&edges, 40);

  VertexIndex *base_index = &mesh->vertex.base.index;
  VertexAttribute *base_attribute = &mesh->vertex.base.attribute;

  for (int i = 0; i < base_index->length; i += 3) {
    unsigned int a = base_index->entries[i];
    unsigned int b = base_index->entries[i + 1];
    unsigned int c = base_index->entries[i + 2];

    EdgeKey ab = {MIN(a, b), MAX(a, b)};
    EdgeKey bc = {MIN(b, c), MAX(b, c)};
    EdgeKey ca = {MIN(a, c), MAX(a, c)};

    edge_hash_set_insert(&edges, ab);
    edge_hash_set_insert(&edges, bc);
    edge_hash_set_insert(&edges, ca);
  }

  // arrays from edges
  size_t vertex_capacity = edges.length * LINE_VERTEX_COUNT * VERTEX_STRIDE;
  vattr_t wireframe_vertex_attribute[vertex_capacity];
  wireframe_vertex->attribute = (VertexAttribute){
      .entries = wireframe_vertex_attribute,
      .capacity = vertex_capacity,
      .length = 0,
      .buffer = NULL,
  };

  size_t index_capacity = edges.length * LINE_INDEX_COUNT;
  vindex_t wireframe_index_attribute[index_capacity];
  wireframe_vertex->index = (VertexIndex){
      .entries = wireframe_index_attribute,
      .capacity = index_capacity,
      .length = 0,
      .buffer = NULL,
  };

  // go through unique edges set add populate temp vertex & index array
  for (size_t l = 0; l < edges.length; l++) {

    size_t index = edges.occupied[l];
    EdgeBucket *current_edge = &edges.entries[index];

    // base vertex
    int base_index = current_edge->key[0];
    float *base_attributes =
        &base_attribute->entries[base_index * VERTEX_STRIDE];
    Vertex base_vertex = vertex_from_array(base_attributes);

    // opposite vertex
    int opp_index = current_edge->key[1];
    float *opp_attributes = &base_attribute->entries[opp_index * VERTEX_STRIDE];
    Vertex opp_vertex = vertex_from_array(opp_attributes);

    // TODO: make dynamic wireframe color
    vec3 color = {0.0f, 1.0f, 0.0f};

    // add points to vertex attributes and index
    line_add_point(base_vertex.position, opp_vertex.position, color,
                   &wireframe_vertex->attribute, &wireframe_vertex->index);
  }

  // upload vertex attribute and index to wireframe buffer

  // upload vertex attributes
  buffer_create(
      &wireframe_vertex->attribute.buffer,
      &(CreateBufferDescriptor){
          .queue = mesh->queue,
          .device = mesh->device,
          .data = (void *)wireframe_vertex->attribute.entries,
          .size = wireframe_vertex->attribute.length * sizeof(vattr_t),
          .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
          .mappedAtCreation = false,
      });

  // upload vertex index
  buffer_create(&wireframe_vertex->index.buffer,
                &(CreateBufferDescriptor){
                    .queue = mesh->queue,
                    .device = mesh->device,
                    .data = (void *)wireframe_vertex->index.entries,
                    .size = wireframe_vertex->index.length * sizeof(vindex_t),
                    .usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });

  // create shader
  shader_create(wireframe_shader,
                &(ShaderCreateDescriptor){
                    .path = "./runtime/assets/shader/shader.line.wgsl",
                    .label = "wireframe",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "wireframe",
                });

  // update pipeline for double-sided
  pipeline_set_primitive(shader_pipeline(mesh_shader_wireframe(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  // freeing wireframe data entries
  wireframe_vertex->attribute.entries = 0;
  wireframe_vertex->index.entries = 0;
}


