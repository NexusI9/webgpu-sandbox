#ifndef _MESH_SHADER_TEXTURE_H_
#define _MESH_SHADER_TEXTURE_H_
#include <stddef.h>
#include <webgpu/webgpu.h>

#include "backend/ubo.h"
#include "core.h"
#include "runtime/camera/camera.h"
#include "runtime/light/light.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/environment/core.h"


void mesh_shader_texture_update_shadow_maps(Mesh *, WGPUTextureView,
                                          WGPUTextureView);

void mesh_shader_texture_update_lights(Mesh *, const MeshShader, UBOManager *);

void mesh_shader_texture_update_probes(Mesh *, WGPUTextureView, WGPUTextureView,
                                       UBOManager *);

void mesh_shader_texture_update_environment(Mesh *, WGPUTextureView,
                                            SceneEnvironmentUniform *,
                                            UBOManager *);

void mesh_shader_texture_bind_probe(Mesh *, ProbeReflectionPlane *,
                                    UBOManager *);

#endif
