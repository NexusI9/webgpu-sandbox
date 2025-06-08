#ifndef _MATERIAL_H_
#define _MATERIAL_H_

/**
   ============================= MATERIAL API ================================

   The material api serve as a bridge between the scene elements, the mesh and
   the shader.

   The material api functions are basically predefined binding layout
   for each standard shaders (texture/ shadow/ wireframe).

   Usually the functions will take a mesh as a first argument.
   Depending on the function, the right standard shader will be automatically
   modified/bound.

   This API serve a more polished and synthesized way to edit and initialize
   standard shaders. Preventing to manually adjust the shader and pipeline.

      .-----------.                                      .-----------------.
      | Scene     |---.      .--------------.      .---> | Texture Shader  |
      '-----------'   |      |     Mesh     |      |     '-----------------'
      .-----------.   |      '--------------'      |
      | Light     |---|             |              |
      '-----------'   |      .--------------.      |     .-----------------.
      .-----------.   +----> | Material API |------+---> | Shadow Shader   |
      | Camera    |---|      '--------------'      |     '-----------------'
      '-----------'   |                            |
      .-----------.   |                            |     .------------------.
      | Viewport  |---'                            '---> | Wireframe Shader |
      '-----------'                                      '------------------'


 */

#include "core.h"
#include "shadow.h"
#include "solid.h"
#include "texture.h"
#include "wireframe.h"
#include "override.h"

#endif
