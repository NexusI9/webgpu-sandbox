#ifndef _SHADER_DRAW_H_
#define _SHADER_DRAW_H_

#include "runtime/shader/core.h"
#include "utils/defines.h"
#include "webgpu/webgpu.h"


EXTERN_C_BEGIN

/**
   Update method called as such: scene update => mesh update => shader update
 */
#ifdef VERBOSE_SHADER_BIND_GROUP_OFFSET
static int bg_offset_count = 0;
static const int bg_print_count = 800;
#endif
static inline void shader_draw(ShaderBindGroupList *bindgroup_list,
                               WGPURenderPipeline pipeline, const char *name,
                               WGPURenderPassEncoder render_pass) {
  
  // bind pipeline to render
  wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);

#ifdef VERBOSE_SHADER_BIND_GROUP_OFFSET
  {
    if (bg_offset_count++ < bg_print_count)
      printf("shader:%s\n", name);
  }
#endif

  for (int i = 0; i < bindgroup_list->length; i++) {

    ShaderBindGroup *bind_group = &bindgroup_list->entries[i];

    // update bindgroup uniforms data
    shader_uniform_update(bind_group);

    // link bind group
    wgpuRenderPassEncoderSetBindGroup(render_pass, i, bind_group->bind_group,
                                      bind_group->offset.count,
                                      bind_group->offset.entries);

#ifdef VERBOSE_SHADER_BIND_GROUP_OFFSET
    {
      if (bg_offset_count < bg_print_count) {
        printf("[%d | %d]", i, bind_group->offset.count);
        for (uint8_t j = 0; j < SHADER_MAX_OFFSET_CAPACITY; j++)
          printf(" %u |", bind_group->offset.entries[j]);
        printf("\n");
      }
    }
#endif
  }
}


EXTERN_C_END

#endif
