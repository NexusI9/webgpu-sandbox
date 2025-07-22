#include "gltf.h"
#include "../../resources/loader/loader.gltf.h"

void example_gltf(Scene *scene) {
  loader_gltf_load(&(GLTFLoadDescriptor){
      .scene = scene,
      .path = "./resources/assets/gltf/cube.gltf",
      .device = scene_device(scene),
      .queue = scene_queue(scene),
      .cgltf_options = &(cgltf_options){0},
  });
}
