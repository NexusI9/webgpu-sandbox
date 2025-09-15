#ifndef _MESH_SHADER_TEXTURE_H_
#define _MESH_SHADER_TEXTURE_H_
#include <stddef.h>
#include <webgpu/webgpu.h>

#include "../backend/ubo.h"
#include "../runtime/camera/camera.h"
#include "../runtime/light/light.h"
#include "../runtime/probe/reflection/plane.h"
#include "core.h"
#include "../backend/ssbo.h"
#include "../runtime/mesh/core.h"

// first bind
void mesh_shader_texture_bind_shadow_maps(Mesh *, WGPUTextureView,
                                          WGPUTextureView);

// update
void mesh_shader_texture_update_lights(Mesh *, const MeshShader, UBOManager *,
                                       SSBOManager *);

void mesh_shader_texture_update_shadow_maps(Mesh *, WGPUTextureView,
                                            WGPUTextureView);

void mesh_shader_texture_update_probes(Mesh *, WGPUTextureView, WGPUTextureView,
                                       SSBOManager *);

void mesh_shader_texture_update_environment(Mesh *, WGPUTextureView,
                                            SSBOManager *);

void mesh_shader_texture_bind_probe(Mesh *, ProbeReflectionPlane *,
                                    SSBOManager *);

#endif
