#ifndef _AO_BAKE_UTILS_H_
#define _AO_BAKE_UTILS_H_

#include "./core.h"
#include "webgpu/webgpu.h"

#ifdef AO_BAKE_HIT_COUNT
extern int g_debug_ao_bake_hit_count;
#endif

bool ao_bake_raycast(const AOBakeRaycastDescriptor *);
float ao_bake_vertex(const AOBakeVertexDescriptor *);
Triangle ao_bake_mesh_triangle(Mesh *, size_t);
void ao_bake_process_texture(Texture *);

#endif
