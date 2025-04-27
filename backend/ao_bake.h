#ifndef _AO_BAKE_H_
#define _AO_BAKE_H_

#include "../runtime/mesh.h"
#include "webgpu/webgpu.h"

#define AO_TEXTURE_SIZE 1024
#define AO_TEXTURE_FORMAT WGPUTextureFormat_R8Unorm
#define AO_DISTANCE 1.0f
#define AO_RAY_AMOUNT 20
#define AO_RAY_MAX_DISTANCE 10.0f

typedef struct {

  MeshIndexedList *mesh_list;

} AOBakeInitDescriptor;

void ao_bake_init(const AOBakeInitDescriptor *);

#endif
