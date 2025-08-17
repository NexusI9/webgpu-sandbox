#ifndef _LOADER_GLTF_H_
#define _LOADER_GLTF_H_

#include "../runtime/scene/scene.h"
#include "cgltf/cgltf.h"
#include <webgpu/webgpu.h>

typedef enum {
  LoaderGLTFStatus_Success,
  LoaderGLTFStatus_UndefError,
  LoaderGLTFStatus_TextureFound,
  LoaderGLTFStatus_TextureUnfound,
  LoaderGLTFStatus_LoadError,
} LoaderGLTFStatus;

typedef struct {
  cgltf_texture_view diffuse;
  cgltf_texture_view metallic;
  cgltf_texture_view normal;
  cgltf_texture_view occlusion;
  cgltf_texture_view emissive;
} GLTFTextureViews;

typedef struct {
  const TextureResolution max_texture_size;
} LoaderGLTFOptions;

typedef struct {
  Scene *scene;
  const char *path;
  const WGPUDevice device;
  const WGPUQueue queue;
  const LoaderGLTFOptions *options;
  const cgltf_options *cgltf_options;
} GLTFLoadDescriptor;

void loader_gltf_load(const GLTFLoadDescriptor *);

#endif
