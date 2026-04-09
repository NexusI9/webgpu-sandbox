#ifndef _AO_BAKE_LOCAL_H_
#define _AO_BAKE_LOCAL_H_

#include "./core.h"

/**
 * @brief      Bake global ambient occlusion to texture
 *
 * @details    The AO Baking process use 2 approaches: Global and Local,
 *             this function handle the Local approach.
 *             It raycast based on hemisphere projection per vertex.
 *
 * @param      storage    The object storing the different textures
 * @param      descriptor The configuration used during the baking process
 *
 */
void ao_bake_local(RendererTextureAO *, const AOBakeLocalDescriptor *desc);

#endif
