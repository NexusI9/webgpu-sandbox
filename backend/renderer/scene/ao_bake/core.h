#ifndef _AO_BAKE_CORE_H_
#define _AO_BAKE_CORE_H_

#include "../runtime/camera/camera.h"
#include "../runtime/geometry/triangle/triangle.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/texture/texture.h"
#include "../runtime/viewport/viewport.h"
#include "../utils/color.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

// AO Texture
#define AO_TEXTURE_SIZE 128
#define AO_TEXTURE_FORMAT WGPUTextureFormat_R8Unorm
#define AO_TEXTURE_CHANNELS TEXTURE_CHANNELS_R
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
  size_t length;
} AOBakeTextureList;

typedef struct {
  WGPUTexture texture;
  AOBakeTextureList texture_list;
  size_t layer_count;
  uint16_t size;
} SceneRendererTextureAO;

typedef struct {
  uint16_t sample_amount;
  float max_distance;
} AOBakeSettings;

typedef struct {
  const WGPUDevice device;
  uint16_t size;
  uint16_t layer_count;
} AOBakeInitDescriptor;

typedef struct {
  MeshList *meshes;
  MeshRefList *pipeline;
  Camera *camera;
  Viewport *viewport;
  color *color;
  size_t max_ray;
} AOBakeDrawDebug;

typedef struct {
  MeshRefList *mesh_list;
  AOBakeSettings local;
  AOBakeSettings global;
  const WGPUDevice device;
  const WGPUQueue queue;
  AOBakeDrawDebug *debug;
} AOBakeDrawDescriptor;

typedef struct {
  MeshRefList *mesh_list;
  Mesh *mesh;
  size_t layer;
  const WGPUDevice device;
  const WGPUQueue queue;
  const AOBakeSettings *settings;
  Texture *texture;
  AOBakeDrawDebug *debug;
} AOBakeGlobalDescriptor;

typedef struct {
  Mesh *mesh;
  const WGPUDevice device;
  const WGPUQueue queue;
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

void ao_bake_init(SceneRendererTextureAO *, const AOBakeInitDescriptor *);

void ao_bake_draw_list(SceneRendererTextureAO *, const AOBakeDrawDescriptor *);

void ao_bake_draw_mesh(SceneRendererTextureAO *, Mesh *,
                       const AOBakeDrawDescriptor *, bool);

#endif
