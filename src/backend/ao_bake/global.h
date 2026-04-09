#ifndef _AO_BAKE_GLOBAL_H_
#define _AO_BAKE_GLOBAL_H_

#include "./core.h"

/**
 * @brief      Bake global ambient occlusion to texture
 *
 * @details    The AO Baking process use 2 approaches: Global and Local,
 *             this function handle the Global approach.
 *             It raycast based on randomly distributed points on each
 *             meshes triangles.
 *
 * @param      storage    The object storing the different textures
 * @param      descriptor The configuration used during the baking process
 *
 */
void ao_bake_global(RendererTextureAO *storage,
                    const AOBakeGlobalDescriptor *desc);

#endif
