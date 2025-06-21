#include "gltf.h"

void example_gltf(Scene *scene, Renderer *renderer) {
  loader_gltf_load(&(GLTFLoadDescriptor){
      .scene = scene,
      .path = "./resources/assets/gltf/cube.gltf",
      .device = renderer_device(renderer),
      .queue = renderer_queue(renderer),
      .cgltf_options = &(cgltf_options){0},
  });
}
