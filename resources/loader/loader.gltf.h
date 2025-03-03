#ifndef _LOADER_GLTF_H_
#define _LOADER_GLTF_H_

#include "../runtime/mesh.h"
#include "cgltf/cgltf.h"

void loader_gltf_load(mesh *, const char *, const cgltf_options *);

#endif
