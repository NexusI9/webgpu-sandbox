#ifndef _AO_BAKE_H_
#define _AO_BAKE_H_

#include "../resources/geometry/triangle.h"
#include "../runtime/mesh.h"
#include "../runtime/scene.h"
#include "../runtime/texture.h"
#include "webgpu/webgpu.h"

// AO Texture
#define AO_TEXTURE_SIZE 128
#define AO_TEXTURE_FORMAT WGPUTextureFormat_R8Unorm
#define AO_TEXTURE_CHANNELS TEXTURE_CHANNELS_R

// Global AO Baking
#define AO_GLOBAL_RAY_AMOUNT 1024
#define AO_GLOBAL_RAY_MAX_DISTANCE 0.05f

// Local AO Baking
#define AO_LOCAL_RAY_AMOUNT 512
#define AO_LOCAL_RAY_MAX_DISTANCE 0.5f

// Debug
#define AO_RAY_MAX_COUNT 10

typedef struct {
  MeshIndexedList *mesh_list;
  scene *scene;
  WGPUDevice *device;
  WGPUQueue *queue;
} AOBakeInitDescriptor;

typedef struct {
  MeshIndexedList *mesh_list;
  texture *texture;
  scene *scene;
  WGPUDevice *device;
  WGPUQueue *queue;
} AOBakeDescriptor;

typedef struct {
  vec3 *ray_origin;
  vec3 *ray_direction;
  triangle *source_triangle;
  texture *source_texture;
  texture *compare_texture;
  mesh *compare_mesh;
} AOBakeRaycastDescriptor;

void ao_bake_init(const AOBakeInitDescriptor *);

#endif
