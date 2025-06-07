#ifndef _AO_BAKE_H_
#define _AO_BAKE_H_

#include "../runtime/geometry/triangle/triangle.h"
#include "../runtime/scene/scene.h"
#include "../runtime/texture/texture.h"

// AO Texture
#define AO_TEXTURE_SIZE 128
#define AO_TEXTURE_FORMAT WGPUTextureFormat_R8Unorm
#define AO_TEXTURE_CHANNELS TEXTURE_CHANNELS_R

// Global AO Baking
#define AO_GLOBAL_RAY_AMOUNT 1024
#define AO_GLOBAL_RAY_MAX_DISTANCE 0.05f

// Local AO Baking
#define AO_LOCAL_RAY_AMOUNT 32
#define AO_LOCAL_RAY_MAX_DISTANCE 0.3f

// Debug
#define AO_RAY_MAX_COUNT 10

typedef struct {
  MeshRefList *mesh_list;
  Scene *scene;
  WGPUDevice *device;
  WGPUQueue *queue;
} AOBakeInitDescriptor;

typedef struct {
  MeshRefList *mesh_list;
  Texture *texture;
  Scene *scene;
  WGPUDevice *device;
  WGPUQueue *queue;
} AOBakeDescriptor;

typedef struct {
  vec3 *ray_origin;
  vec3 *ray_direction;
  Triangle *source_triangle;
  Texture *source_texture;
  Texture *compare_texture;
  Mesh *compare_mesh;
} AOBakeRaycastDescriptor;

void ao_bake_init(const AOBakeInitDescriptor *);

#endif
