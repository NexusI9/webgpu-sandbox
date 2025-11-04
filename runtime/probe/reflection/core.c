#include "core.h"

#include <stdbool.h>
#include <webgpu/webgpu.h>

#include "backend/context.h"
#include "backend/renderer/render_pass/core.h"
#include "backend/resource_manager.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "utils/dyli.h"

DynamicListStatus
probe_reflection_list_create_core(const ProbeReflectionCreateList *desc) {

  return dyli_create((void *)desc->entries, desc->capacity, desc->length,
                     desc->type_size, desc->num, desc->label);
}

