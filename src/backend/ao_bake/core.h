#ifndef _AO_BAKE_CORE_H_
#define _AO_BAKE_CORE_H_

#include <cglm/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "runtime/camera/camera.h"
#include "runtime/geometry/triangle/core.h"
#include "runtime/geometry/triangle/triangle.h"
#include "runtime/geometry/vertex/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/mesh.h"
#include "runtime/scene/debug/core.h"
#include "runtime/scene/debug/debug.h"
#include "runtime/texture/core.h"
#include "runtime/texture/texture.h"
#include "runtime/viewport/viewport.h"
#include "utils/color.h"
#include "webgpu/webgpu.h"

// AO Texture
#define AO_TEXTURE_RESOLUTION TextureResolution_128
#define AO_TEXTURE_FORMAT WGPUTextureFormat_R8Unorm
#define AO_TEXTURE_CHANNELS TextureChannel_R
#define AO_LAYER_COUNT 24

// Global AO Baking
#define AO_GLOBAL_RAY_AMOUNT 1024
#define AO_GLOBAL_RAY_MAX_AMOUNT 1024
#define AO_GLOBAL_RAY_MAX_DISTANCE 0.1f

// Local AO Baking
#define AO_LOCAL_RAY_AMOUNT 32
#define AO_LOCAL_RAY_MAX_AMOUNT 1024
#define AO_LOCAL_RAY_MAX_DISTANCE 0.7f

// Debug
#define AO_RAY_MAX_COUNT 10

typedef struct {
  Texture texture;
  Mesh *owner;
} AOBakeTextureListEntry;

typedef struct {
  AOBakeTextureListEntry *entries;
  size_t capacity;
  size_t count;
} AOBakeTextureList;

typedef struct {
  WGPUTexture texture;
  AOBakeTextureList texture_list;
  size_t layer_count;
  uint16_t size;
} RendererTextureAO;

typedef struct {
  uint16_t sample_amount;
  float max_distance;
} AOBakeSettings;

typedef struct {

  uint16_t size;
  uint16_t layer_count;
} AOBakeInitDescriptor;

typedef struct {
  SceneDebug *debug_scene;
  color *color;
  size_t max_ray;
} AOBakeDrawDebug;

typedef struct {
  MeshRefList *mesh_list;
  AOBakeSettings local;
  AOBakeSettings global;

  AOBakeDrawDebug *debug;
} AOBakeDrawDescriptor;

typedef struct {
  MeshRefList *mesh_list;
  Mesh *mesh;
  size_t layer;

  const AOBakeSettings *settings;
  Texture *texture;
  AOBakeDrawDebug *debug;
} AOBakeGlobalDescriptor;

typedef struct {
  Mesh *mesh;

  const AOBakeSettings *settings;
  Texture *texture;
  AOBakeDrawDebug *debug;
} AOBakeLocalDescriptor;

typedef struct {
  vec3 *ray_origin;
  vec3 *ray_direction;
  Triangle *source_triangle;
  Texture *source_texture;
  Texture *compare_texture;
  Mesh *compare_mesh;
  const float max_distance;
  const uint16_t texture_size;
} AOBakeRaycastDescriptor;

typedef struct {
  Vertex *vertex;
  Mesh *mesh;
  const AOBakeSettings *settings;
  struct {
    int max_ray;
    Mesh *line;
  } debug;
} AOBakeVertexDescriptor;

/**
 * @brief      Initialise AO baking necessary resources
 *
 * @param      storage    The object storing the different textures
 * @param      descriptor The configuration and settings
 *
 */
void ao_bake_init(RendererTextureAO *storage,
                  const AOBakeInitDescriptor *descriptor);

/**
 * @brief      Draw and compoute AO for the meshes provided in the descriptor
 *
 * @param      storage    The object storing the different textures
 * @param      descriptor The configuration and settings
 *
 */
void ao_bake_draw_list(RendererTextureAO *storage,
                       const AOBakeDrawDescriptor *descriptor);

/**
 * @brief      Draw and compoute AO for the the given mesh
 *
 * @param      storage    The object storing the different textures
 * @param      mesh       The mesh to draw AO from
 * @param      descriptor The configuration and settings
 * @param      write      Write the computed AO onto the texture 
 *
 */
void ao_bake_draw_mesh(RendererTextureAO *storage, Mesh *mesh,
                       const AOBakeDrawDescriptor *descriptor,
                       bool write);

#endif
