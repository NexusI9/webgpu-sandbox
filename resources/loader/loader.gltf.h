#ifndef _LOADER_GLTF_H_
#define _LOADER_GLTF_H_

#include "../runtime/scene/scene.h"
#include "cgltf/cgltf.h"
#include <webgpu/webgpu.h>


#define LOADER_GLTF_SUCCESS 0
#define LOADER_GLTF_UNDEF_ERROR 1
#define LOADER_GLTF_TEXTURE_FOUND 2
#define LOADER_GLTF_TEXTURE_UNFOUND 3
#define LOADER_GLTF_TEXTURE_LOAD_ERROR 4

typedef struct {
  cgltf_texture_view diffuse;
  cgltf_texture_view metallic;
  cgltf_texture_view normal;
  cgltf_texture_view occlusion;
  cgltf_texture_view emissive;
} GLTFTextureViews;

typedef struct {
  Scene *scene;
  const char *path;
  WGPUDevice *device;
  WGPUQueue *queue;
  const cgltf_options *cgltf_options;
} GLTFLoadDescriptor;

void loader_gltf_load(const GLTFLoadDescriptor *);

#endif
