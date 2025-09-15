#ifndef _PREFAB_SKYBOX_H_
#define _PREFAB_SKYBOX_H_

#include <stddef.h>

#include "../../texture/texture.h"
#include "../prefab.h"
#include "../runtime/scene/core.h"
#include "../runtime/texture/core.h"
#include "../runtime/texture/create.h"
#include "../runtime/texture/write.h"
#include "../runtime/scene/core.h"
#include "../runtime/texture/core.h"
#include "../runtime/texture/create.h"
#include "../runtime/texture/write.h"

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
