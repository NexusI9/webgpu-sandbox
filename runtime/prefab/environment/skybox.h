#ifndef _PREFAB_SKYBOX_H_
#define _PREFAB_SKYBOX_H_

#include "../../texture/texture.h"
#include "../prefab.h"

typedef struct {
  const char *right;
  const char *left;
  const char *top;
  const char *bottom;
  const char *front;
  const char *back;
} CubeMapPath;

typedef struct {

  float blur;
  size_t resolution;
  CubeMapPath path;

} PrefabSkyboxCreateDescriptor;

typedef struct {
  size_t resolution;
  TextureGradient stops;
} PrefabSkyboxGradientCreateDescriptor;

void prefab_skybox_create(const PrefabCreateDescriptor *,
                          const PrefabSkyboxCreateDescriptor *);

void prefab_skybox_gradient_create(
    const PrefabCreateDescriptor *,
    const PrefabSkyboxGradientCreateDescriptor *);
#endif
