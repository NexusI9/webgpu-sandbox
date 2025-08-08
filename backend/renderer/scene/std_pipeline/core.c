#include "core.h"
#include "../runtime/pipeline/pipeline.h"
#include "./layouts/layout.billboard.h"
#include "./layouts/layout.default.h"
#include "./layouts/layout.grid.h"
#include "./layouts/layout.line.h"
#include "./layouts/layout.pbr.h"
#include "./layouts/layout.screen.h"
#include "./layouts/layout.shadow.h"
#include "./layouts/layout.skybox.h"
#include "./layouts/layout.solid.h"
#include "./layouts/layout.unlit.h"
#include "webgpu/webgpu.h"

static const PipelineLayoutDescriptor *standard_layouts[PIPELINE_TYPE_COUNT] = {
    [PipelineType_Billboard] = &layout_billboard,
    [PipelineType_Default] = &layout_default,
    [PipelineType_Line] = &layout_line,
    [PipelineType_Unlit] = &layout_unlit,
    [PipelineType_Grid] = &layout_grid,
    [PipelineType_PBR] = &layout_pbr,
    [PipelineType_Screen] = &layout_screen,
    [PipelineType_Shadow] = &layout_shadow,
    [PipelineType_ShadowCullBack] = &layout_shadow_cullback,
    [PipelineType_Skybox] = &layout_skybox,
    [PipelineType_Solid] = &layout_solid,
};

Pipeline g_std_pipelines[PIPELINE_TYPE_COUNT] = {0};

/**
   Initialize standards shaders and build pipelines layout for each of them.
 */
void standard_pipelines_init(const WGPUDevice device,
                             const PipelineMultisampleCount multisample) {

  VERBOSE_PROCESS("Initializing standards pipelines...");

  for (size_t i = 0; i < PIPELINE_TYPE_COUNT; i++) {

    const PipelineLayoutDescriptor *layout = standard_layouts[i];
    Pipeline *cached_pipeline = &g_std_pipelines[i];

    // create pipeline
    pipeline_create(cached_pipeline, &(PipelineCreateDescriptor){
                                         .device = device,
                                         .label = layout->label,
                                         .path = layout->shader_path,
                                     });

    // transfert original layout descriptor so empty shader bindgroups can be
    // generated from it
    cached_pipeline->layout_descriptor = layout;

    // check custom attributes (weak check)

    // vertex state
    if (layout->pipeline_attributes.vertex_state.module != NULL)
      pipeline_set_vertex(cached_pipeline,
                          layout->pipeline_attributes.vertex_state);

    // fragment state
    if (layout->pipeline_attributes.fragment_state.color_state.format !=
        WGPUTextureFormat_Undefined)
      pipeline_set_fragment(cached_pipeline,
                            &layout->pipeline_attributes.fragment_state);

    // primitive state
    if (layout->pipeline_attributes.primitive_state.cullMode !=
        WGPUCullMode_Undefined)
      pipeline_set_primitive(cached_pipeline,
                             layout->pipeline_attributes.primitive_state);
    // stencil state
    if (layout->pipeline_attributes.stencil_state.format !=
        WGPUTextureFormat_Undefined)
      pipeline_set_stencil(cached_pipeline,
                           layout->pipeline_attributes.stencil_state);

    // blend state
    if (layout->pipeline_attributes.blend_state.alpha.dstFactor)
      pipeline_set_blend(cached_pipeline,
                         &layout->pipeline_attributes.blend_state);

    // if sampling set in custom attbutes, apply the config one
    if (layout->pipeline_attributes.multisample !=
        PipelineMultisampleCount_Undefined) {
      pipeline_set_sampling(cached_pipeline,
                            layout->pipeline_attributes.multisample);
    } else {
      // else use the renderer one
      pipeline_set_sampling(cached_pipeline, multisample);
    }

    // copy std bindings (mvp)
    cached_pipeline->bindings = layout->bindings;

    // build layout based on bindgroup description
    WGPUPipelineLayout temp_layout = pipeline_layout_descriptor_create(
        layout->bind_groups, layout->bind_groups_count, device, NULL);

    pipeline_build(cached_pipeline, &temp_layout);
  }
}

const Pipeline *std_pipeline(const PipelineType type) {
  return &g_std_pipelines[type];
}
