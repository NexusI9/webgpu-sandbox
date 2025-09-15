#include "gltf.h"

#include <cgltf/cgltf.h>
#include <stddef.h>

#include "resources/loader/loader.gltf.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/mesh/shader/texture.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/add.h"
#include "runtime/texture/core.h"

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

  const char *podium_name = "podium";
  Mesh *podium = mesh_ref_list_find_by_name(
      &(MeshRefList){
          .capacity = gltf_result.meshes.length,
          .length = gltf_result.meshes.length,
          .entries = gltf_result.meshes.entries,
      },
      podium_name);

  if (podium)
    mesh_shader_texture_bind_probe(podium, plane, &scene->renderer.ssbo);
}
