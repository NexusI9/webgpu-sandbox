#ifndef _PREFAB_SKYBOX_H_
#define _PREFAB_SKYBOX_H_

#include "../../texture/texture.h"
#include "../prefab.h"

typedef struct {
  float blur;
  TextureResolution resolution;
  CubeMapPath path;

} PrefabSkyboxCreateDescriptor;

typedef struct {
  size_t resolution;
  TextureGradient stops;
} PrefabSkyboxGradientCreateDescriptor;

void prefab_skybox_create(Scene *, const PrefabSkyboxCreateDescriptor *);

void prefab_skybox_gradient_create(
    Scene *, const PrefabSkyboxGradientCreateDescriptor *);
#endif
