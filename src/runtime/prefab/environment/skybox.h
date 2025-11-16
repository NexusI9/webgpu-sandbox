#ifndef _PREFAB_SKYBOX_H_
#define _PREFAB_SKYBOX_H_

#include <stddef.h>

#include "backend/compute/mipmap.h"
#include "runtime/engine/core.h"
#include "runtime/prefab/prefab.h"
#include "runtime/scene/core.h"
#include "runtime/texture/core.h"
#include "runtime/texture/create.h"
#include "runtime/texture/texture.h"
#include "runtime/texture/write.h"

typedef struct {
  const mip_t blur;
  const TextureResolution resolution;
  const CubeMapPath path;
} PrefabSkyboxCreateDescriptor;

typedef struct {
  const size_t resolution;
  const TextureGradient stops;
} PrefabSkyboxGradientCreateDescriptor;

void prefab_skybox_create(Engine *engine, const PrefabSkyboxCreateDescriptor *);

void prefab_skybox_gradient_create(
    Engine *engine, const PrefabSkyboxGradientCreateDescriptor *);
#endif
