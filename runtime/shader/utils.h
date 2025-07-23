#ifndef _SHADER_UTILS_H_
#define _SHADER_UTILS_H_

#include "core.h"
#define SHADER_BIND_VALID 0
#define SHADER_BIND_UNVALID 1

uint16_t shader_bind_group_entries_count(const ShaderBindGroup*);

bool shader_validate_binding(Shader *);
#endif
