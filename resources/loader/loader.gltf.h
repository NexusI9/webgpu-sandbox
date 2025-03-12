#ifndef _LOADER_GLTF_H_
#define _LOADER_GLTF_H_

#include "../runtime/mesh.h"
#include "cgltf/cgltf.h"

typedef struct {
  cgltf_texture_view diffuse;
  cgltf_texture_view metallic;
  cgltf_texture_view normal;
  cgltf_texture_view occlusion;
  cgltf_texture_view emissive;
} GLTFTextureViews;

void loader_gltf_load(mesh *, const char *, const cgltf_options *);

#endif
