#include "build.h"
#include "../material/material.h"
#include "../utils/system.h"
#include <stdint.h>

// topology methods
static void scene_build_mesh_create_topology_wireframe(Scene *, MeshRefList *);
static void scene_build_mesh_create_topology_boundbox_bound(Scene *,
                                                            MeshRefList *);
static void scene_build_mesh_create_topology_boundbox_full(Scene *,
                                                           MeshRefList *);

// shader methods
static void scene_build_mesh_create_dynamic_shader(
    Scene *, mesh_create_dynamic_shader_callback, MeshRefList *);

static void scene_build_mesh_bind_views(Scene *, material_bind_views_callback,
                                        uint8_t, MeshRefList *);
// pipeline methods
static void scene_build_mesh_list(Scene *, mesh_get_shader_callback,
                                  PipelineMultisampleCount, MeshRefList *);

/**
   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_texture(Scene *scene, PipelineMultisampleCount sample) {
  VERBOSE_PRINT("======= BUILD TEXTURE SCENE ======\n");

  MeshRefList *layers[3] = {
      &scene->pipelines.background,
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  const int lit_pipeline_index = 1;

  for (size_t l = 0; l < 3; l++) {
    MeshRefList *layer = layers[l];

    // compute boundbox bounds for collisions (lightweight)
    scene_build_mesh_create_topology_boundbox_bound(scene, layer);

    // bind views
    scene_build_mesh_bind_views(scene, material_texture_bind_views,
                                SHADER_TEXTURE_BINDGROUP_VIEWS, layer);

    // bind lights (lit only)
    if (l == lit_pipeline_index) {
      for (int i = 0; i < layer->length; i++) {
        material_texture_bind_lights(layer->entries[i], &scene->lights,
                                     SHADER_TEXTURE_BINDGROUP_LIGHTS);
      }
    }

    // build layer
    scene_build_mesh_list(scene, mesh_shader_texture, sample, layer);
  }

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_solid(Scene *scene, PipelineMultisampleCount sample) {
  VERBOSE_PRINT("======= BUILD SOLID SCENE ======\n");

  MeshRefList *layers[2] = {
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 2; l++) {
    MeshRefList *layer = layers[l];

    // compute boundbox bounds for collisions (lightweight)
    scene_build_mesh_create_topology_boundbox_bound(scene, layer);

    // create meshes' solid shader
    scene_build_mesh_create_dynamic_shader(scene, mesh_create_solid_shader,
                                           layer);

    // bind views
    scene_build_mesh_bind_views(scene, material_solid_bind_views,
                                SHADER_SOLID_BINDGROUP_VIEWS, layer);

    // build solid meshes first
    scene_build_mesh_list(scene, mesh_shader_solid, sample, layer);
  }

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_wireframe(Scene *scene, PipelineMultisampleCount sample) {
  VERBOSE_PRINT("======= BUILD WIREFRAME SCENE ======\n");

  MeshRefList *layers[2] = {
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 2; l++) {
    MeshRefList *layer = layers[l];

    // compute boundbox bounds for collisions (lightweight)
    scene_build_mesh_create_topology_boundbox_bound(scene, layer);

    // create wireframe topology
    scene_build_mesh_create_topology_wireframe(scene, layer);

    // create meshes' wireframe shader
    scene_build_mesh_create_dynamic_shader(scene, mesh_create_wireframe_shader,
                                           layer);

    // bind views
    scene_build_mesh_bind_views(scene, material_wireframe_bind_views,
                                SHADER_WIREFRAME_BINDGROUP_VIEWS, layer);

    // build "solid" meshes first
    scene_build_mesh_list(scene, mesh_shader_wireframe, sample, layer);
  }

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build meshes Boundbox shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_boundbox(Scene *scene, PipelineMultisampleCount sample) {
  VERBOSE_PRINT("======= BUILD BOUNDBOX SCENE ======\n");

  MeshRefList *layers[2] = {
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 2; l++) {
    MeshRefList *layer = layers[l];

    // create full boundbox topology
    scene_build_mesh_create_topology_boundbox_full(scene, layer);

    // create meshes' wireframe shader
    scene_build_mesh_create_dynamic_shader(scene, mesh_create_wireframe_shader,
                                           layer);

    // bind views
    scene_build_mesh_bind_views(scene, material_wireframe_bind_views,
                                SHADER_WIREFRAME_BINDGROUP_VIEWS, layer);

    // build "solid" meshes first
    scene_build_mesh_list(scene, mesh_shader_wireframe, sample, layer);
  }

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build meshes Shadow shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_shadow(Scene *scene, PipelineMultisampleCount sample) {
  VERBOSE_PRINT("======= BUILD SHADOW SCENE ======\n");

  // (shadow is bind process already during the renderer shadow pass)

  MeshRefList *layers[2] = {
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 2; l++) {
    MeshRefList *layer = layers[l];
    scene_build_mesh_list(scene, mesh_shader_shadow, sample, layer);
  }

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Override shader* as default shader.
 */
void scene_build_fixed(Scene *scene, PipelineMultisampleCount sample) {
  VERBOSE_PRINT("======= BUILD FIXED SCENE ======\n");

  MeshRefList *layer = &scene->pipelines.fixed;

  // compute boundbox bounds for collisions (lightweight)
  scene_build_mesh_create_topology_boundbox_bound(scene, layer);

  // bind views
  scene_build_mesh_bind_views(scene, material_override_bind_views,
                              SHADER_FIXED_BINDGROUP_VIEWS, layer);

  // build fixed
  scene_build_mesh_list(scene, mesh_shader_override, sample, layer);

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
    ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▖    ▗▄▄▖
    ▐▌ ▐▌  █    █  ▐▌   ▐▌
    ▐▌ ▐▌  █    █  ▐▌    ▝▀▚▖
    ▝▚▄▞▘  █  ▗▄█▄▖▐▙▄▄▖▗▄▄▞▘

 */

void scene_build_mesh_list(Scene *scene, mesh_get_shader_callback target_shader,
                           PipelineMultisampleCount sample,
                           MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];

    // updating meshes shader's pipeline sampling (dirty)
    pipeline_set_sampling(shader_pipeline(target_shader(current_mesh)), sample);

    // build shader pipeline
    mesh_build(current_mesh, target_shader(current_mesh));

    // release shader module
    shader_module_release(target_shader(current_mesh));
  }
}

/**
   Create mesh's extra shader for the dynamic rendering (wireframe/ shadow)
   Dynamic shaders are only used in lit and unlit scene layers
 */
void scene_build_mesh_create_dynamic_shader(
    Scene *scene, mesh_create_dynamic_shader_callback target_shader_creator,
    MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    target_shader_creator(current_mesh);
  }
}

/**
   Create mesh's extra topology (wireframe/bounding box...)
   All extra topology are derived from the mesh's base topology and manipulate
   it accordingly to create the new topology.
 */
void scene_build_mesh_create_topology_wireframe(Scene *scene,
                                                MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *mesh = mesh_list->entries[i];
    MeshTopology src_topo = mesh_topology_base_vertex(&mesh->topology.base);
    MeshTopologyWireframe *dest_topo = &mesh->topology.wireframe;

    mesh_topology_wireframe_create(&src_topo, dest_topo, mesh->device,
                                   mesh->queue);
  }
}

/**
   Create whole meshes topology (bound + cube wireframe)
 */
void scene_build_mesh_create_topology_boundbox_full(Scene *scene,
                                                    MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *mesh = mesh_list->entries[i];
    MeshTopologyBoundbox *dest_topo = &mesh->topology.boundbox;
    mesh_topology_boundbox_create(&mesh->topology.base, mesh->model, dest_topo,
                                  mesh->device, mesh->queue);
  }
}

/**
   Compute meshes boundbox min and max, doesn't create the whole cube
   topology.
 */
void scene_build_mesh_create_topology_boundbox_bound(Scene *scene,
                                                     MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *mesh = mesh_list->entries[i];
    MeshTopologyBoundbox *dest_topo = &mesh->topology.boundbox;
    mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                         dest_topo);
  }
}

void scene_build_mesh_bind_views(Scene *scene,
                                 material_bind_views_callback bind_callback,
                                 uint8_t group_id, MeshRefList *mesh_list) {

  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *mesh = mesh_list->entries[i];
    bind_callback(mesh, scene->active_camera, &scene->viewport, group_id);
  }
}
