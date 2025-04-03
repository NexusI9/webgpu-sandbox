#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "mesh.h"

/**
   ============================= MATERIAL API ================================

   The material api serve as a bridge between the scene elements, the mesh and
   the shader.

   The material api functions are basically predefined binding layout
   for each standard shaders (texture/ shadow/ wireframe).

   Usually the functions will take a mesh as a first argument.
   Depending on the function, the right standard shader will be automatically
   modified/bound.


      +-----------+                                    +-----------------+
      | Scene     |---.      +--------------+      .---| Texture Shader  |
      +-----------+   |      |     Mesh     |      |   +-----------------+
      +-----------+   |      +--------------+      |
      | Light     |---|             |              |
      +-----------+   |      +--------------+      |   +-----------------+
      +-----------+   +------| Material API |------+---| Shadow Shader   |
      | Camera    |---|      +--------------+      |   +-----------------+
      +-----------+   |                            |
      +-----------+   |                            |   +------------------+
      | Viewport  |---'                            '---| Wireframe Shader |
      +-----------+                                    +------------------+


 */

// COMMON
void material_clear_bindings(mesh *, MeshDrawMethod);

// === TEXTURE SHADER ===
// bind model, camera and viewport to bind group
void material_bind_views(mesh *, camera *, viewport *, uint8_t);
// bind light scene
void material_bind_lights(mesh *, viewport *, AmbientLightList *,
                          DirectionalLightList *, PointLightList *, uint8_t);

// === SHADOW SHADER ===
// bind shadow specicif view
void material_bind_shadow_views(mesh *, mat4 *);
void material_bind_shadow_maps(mesh *, WGPUTextureView *);

#endif
