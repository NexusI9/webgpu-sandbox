#ifndef _MESH_SHADER_TEXTURE_H_
#define _MESH_SHADER_TEXTURE_H_
#include "../backend/ubo.h"
#include "../runtime/camera/camera.h"
#include "../runtime/light/light.h"
#include "core.h"
#include <stddef.h>

// first bind
void mesh_shader_texture_bind_shadow_maps(Mesh *, WGPUTextureView,
                                          WGPUTextureView);

// update
void mesh_shader_texture_update_lights(Mesh *, UBOManager *, SSBOManager *);
void mesh_shader_texture_update_shadow_maps(Mesh *, WGPUTextureView,
                                            WGPUTextureView);

// pipelines
void mesh_shader_texture_double_sided(Mesh *);

// clear
void mesh_shader_texture_clear_bindings(Mesh *);

#endif
