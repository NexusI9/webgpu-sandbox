#ifndef _PREFAB_H_
#define _PREFAB_H_

#include "../scene/scene.h"
#include <webgpu/webgpu.h>

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  Scene *scene;
} PrefabCreateDescriptor;

#endif
