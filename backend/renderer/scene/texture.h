#ifndef _SCENE_RENDERER_TEXTURE_H_
#define _SCENE_RENDERER_TEXTURE_H_

#include "core.h"
#include "../runtime/texture/texture.h"


// textures initializer called in the scene_renderer_create (./core.c)
void scene_renderer_init_fallback_textures(SceneRenderer *);

#endif
