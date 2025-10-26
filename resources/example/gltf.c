#include "gltf.h"

#include <cgltf/cgltf.h>
#include <stddef.h>

#include "backend/context.h"
#include "resources/loader/loader.gltf.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/mesh/shader/texture.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/texture/core.h"

void example_gltf(Scene *scene) {
  loader_gltf_load(
      &(GLTFLoadDescriptor){
          .scene = scene,
          .path = "./resources/assets/gltf/cube.gltf",
          .cgltf_options = &(cgltf_options){0},
          .options =
              &(LoaderGLTFOptions){
                  .max_texture_size = TextureResolution_512,
              },
      },
      NULL);
}

void example_gltf_spa(Scene *scene) {

  //  scene_add_point_light(scene,
  //                        &(PointLightDescriptor){
  //                            .color = {0.4f, 0.0f, 1.0f, 1.0f},
  //                            .intensity = 7.0f,
  //                            .cutoff = 20.0f,
  //                            .inner_cutoff = 50.0f,
  //                            .near = 0.1,
  //                            .far = 20.0f,
  //                            .position = {0.0f, 3.5f, 0.0f},
  //                        },
  //                        LightCreateFlag_None, NULL);

  scene_add_sun_light(
      scene,
      &(SunLightDescriptor){
          .position = {10.8f, 12.0f, -15.0f},
          .color = {241.0f / 255.0f, 120.0f / 255.0f, 82.0f / 255.0f, 1.0f},
          .intensity = 3.2f,
          .size = 10.0f,
      },
      LightCreateFlag_None, NULL);

  scene_add_ambient_light(scene,
                          &(AmbientLightDescriptor){
                              .color = {0.0f, 0.4f, 1.0f, 1.0f},
                              .intensity = 0.2f,
                              .position = {-2.0f, 3.0f, 3.3f},
                          },
                          NULL);

  loader_gltf_load(
      &(GLTFLoadDescriptor){
          .scene = scene,
          .path = "./resources/assets/gltf/spa.gltf",
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
          .cgltf_options = &(cgltf_options){0},
          .options =
              &(LoaderGLTFOptions){
                  .max_texture_size = TextureResolution_512,
              },
      },
      &gltf_result);

  // create a new planar reflection for the podium
  ProbeReflectionPlane *plane;
  SceneEditorMeshList *plane_probe =
      scene_add_probe_reflection_plane(scene,
                                       &(ProbeReflectionPlaneDescriptor){
                                           .far = 100.0f,
                                           .near = 0.1f,
                                           .normal = {0.0f, 1.0f, 0.0f},
                                           .position = {0.0f, 1.0f, 0.0f},
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
    mesh_shader_texture_bind_probe(podium, plane, &scene->renderer.ubo);
}
