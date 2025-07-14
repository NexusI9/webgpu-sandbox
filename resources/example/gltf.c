#include "gltf.h"

void example_gltf(Scene *scene) {
  loader_gltf_load(&(GLTFLoadDescriptor){
      .scene = scene,
      .path = "./resources/assets/gltf/cube.gltf",
      .device = scene->device,
      .queue = scene->queue,
      .cgltf_options = &(cgltf_options){0},
  });
}
