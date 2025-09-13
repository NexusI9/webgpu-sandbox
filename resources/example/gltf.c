#include "gltf.h"
#include "../../resources/loader/loader.gltf.h"
#include "../backend/std_pipeline/modules/glass_probe_plane/glass_probe_plane.h"
#include "../runtime/mesh/shader/shader.h"

void example_gltf(Scene *scene) {
  loader_gltf_load(
      &(GLTFLoadDescriptor){
          .scene = scene,
          .path = "./resources/assets/gltf/cube.gltf",
          .device = scene_device(scene),
          .queue = scene_queue(scene),
          .cgltf_options = &(cgltf_options){0},
          .options =
              &(LoaderGLTFOptions){
                  .max_texture_size = TextureResolution_512,
              },
      },
      NULL);
}

void example_gltf_podium(Scene *scene) {

  LoaderGLTFResult gltf_result;
  LoaderGLTFStatus status = loader_gltf_load(
      &(GLTFLoadDescriptor){
          .scene = scene,
          .path = "./resources/assets/gltf/podium.gltf",
          .device = scene_device(scene),
          .queue = scene_queue(scene),
          .cgltf_options = &(cgltf_options){0},
          .options =
              &(LoaderGLTFOptions){
                  .max_texture_size = TextureResolution_512,
              },
      },
      &gltf_result);

  // create a new planar reflection for the podium
  ProbeReflectionPlane *plane;
  SceneEditorObject *plane_probe =
      scene_add_probe_reflection_plane(scene,
                                       &(ProbeReflectionPlaneDescriptor){
                                           .far = 100.0f,
                                           .near = 0.1f,
                                           .normal = {0.0f, 1.0f, 0.0f},
                                           .position = {0.0f, 2.0f, 0.0f},
                                           .scale = {10.0f, 10.0f, 10.0f},
                                           .distance = 1.0f,
                                           .camera = scene->active_camera,
                                       },
                                       &plane);

  const char *podium_name = "Circle.001";
  Mesh *podium = mesh_ref_list_find_by_name(
      &(MeshRefList){
          .capacity = gltf_result.meshes.length,
          .length = gltf_result.meshes.length,
          .entries = gltf_result.meshes.entries,
      },
      podium_name);

  if (podium) {
    shader_update_uniform_data(mesh_shader(podium, MeshShader_Texture), 1, 0,
                               &(GlassUniform){
                                   .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                   .frost_scale = 20.0f,
                                   .frost_strength = 2.0f,
                                   .roughness = 0.145f,
                               });

    printf("plane id: %lu\n",
           plane->ssbo_slot[ProbeReflectionSSBOField_List].id);

    printf("plane texture layer: %u\n", plane->texture_layer);

    mesh_shader_texture_bind_probe(podium, plane, &scene->renderer.ssbo);
  }
}
