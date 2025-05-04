#ifndef _LOADER_GLTF_H_
#define _LOADER_GLTF_H_

#include "../runtime/scene.h"
#include "cgltf/cgltf.h"
#include <webgpu/webgpu.h>

typedef struct {
  cgltf_texture_view diffuse;
  cgltf_texture_view metallic;
  cgltf_texture_view normal;
  cgltf_texture_view occlusion;
  cgltf_texture_view emissive;
} GLTFTextureViews;

typedef struct {
  scene *scene;
  const char *path;
  WGPUDevice *device;
  WGPUQueue *queue;
  const cgltf_options *cgltf_options;
} GLTFLoadDescriptor;

void loader_gltf_load(const GLTFLoadDescriptor *);

#endif
