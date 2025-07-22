#ifndef _SCENE_MESH_BUILD_H_
#define _SCENE_MESH_BUILD_H_
#include "core.h"

/**
   Mesh Building process:

   The scene building process handles each layers respective essentials shader
   creation or binding process( view matrix...).

   Currently the renderer handles different passes such as :
   - Topology Creation
   - Shader creation
   - Shader bind views
   - Shader bind lights
   - Shader build pipeline layout
 */

void scene_build_mesh(Scene*, Mesh*, const ScenePipeline);

#endif
