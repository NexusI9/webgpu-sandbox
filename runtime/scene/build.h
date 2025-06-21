#ifndef _SCENE_BUILD_H_
#define _SCENE_BUILD_H_
#include "core.h"

/**
   Scene Building process:

   The scene building process handles each layers respective essentials shader
   creation or binding process( view matrix...).

   Currently the renderer handles different passes such as :
   - Texture: PBR shader
   - Shadow: Shadow shader
   - Solid : Solid shader
   - Wireframe : Wireframe Shader + new vertex armature creation
   - Fixed : custom fixed shader

 */

void scene_build_texture(Scene *, PipelineMultisampleCount);
void scene_build_shadow(Scene *, PipelineMultisampleCount);
void scene_build_solid(Scene *, PipelineMultisampleCount);
void scene_build_wireframe(Scene *, PipelineMultisampleCount);
void scene_build_fixed(Scene *, PipelineMultisampleCount);

#endif
