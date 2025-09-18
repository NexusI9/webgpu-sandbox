#include "core.h"

#include <stddef.h>

#include "./render_shader/billboard/billboard.h"
#include "./render_shader/default/default.h"
#include "./render_shader/glass_probe_grid/glass_probe_grid.h"
#include "./render_shader/glass_probe_plane/glass_probe_plane.h"
#include "./render_shader/grid/grid.h"
#include "./render_shader/line/line.h"
#include "./render_shader/pbr/pbr.h"
#include "./render_shader/reflection/reflection.h"
#include "./render_shader/screen/screen.h"
#include "./render_shader/shadow/shadow.h"
#include "./render_shader/skybox/skybox.h"
#include "./render_shader/solid/solid.h"
#include "./render_shader/unlit/unlit.h"
#include "backend/std_pipeline/compute_shader/kawase/kawase.h"
#include "backend/std_pipeline/compute_shader/mipmap/mipmap.h"
#include "runtime/pipeline/render.h"
#include "runtime/pipeline/set.h"
#include "utils/system.h"
#include "webgpu/webgpu.h"

// Global definitions
RenderPipeline g_std_render_pipelines[RENDER_PIPELINE_TYPE_COUNT] = {0};
ComputePipeline g_std_compute_pipelines[COMPUTE_PIPELINE_TYPE_COUNT] = {0};

static inline WGPUPipelineLayout shader_pipeline_state_object_create(
    const WGPUBindGroupLayoutDescriptor *const *, const size_t,
    const WGPUDevice, WGPUBindGroupLayout *);

static const RenderPipelineStateObject
    *standard_render_layouts[RENDER_PIPELINE_TYPE_COUNT] = {
        [RenderPipelineType_Billboard] = &layout_billboard,
        [RenderPipelineType_Default] = &layout_default,
        [RenderPipelineType_Line] = &layout_line,
        [RenderPipelineType_Unlit] = &layout_unlit,
        [RenderPipelineType_Grid] = &layout_grid,
        [RenderPipelineType_PBR] = &layout_pbr,
        [RenderPipelineType_Screen] = &layout_screen,
        [RenderPipelineType_Shadow] = &layout_shadow,
        [RenderPipelineType_ShadowCullBack] = &layout_shadow_cullback,
        [RenderPipelineType_Skybox] = &layout_skybox,
        [RenderPipelineType_Solid] = &layout_solid,
        [RenderPipelineType_GlassProbeGrid] = &layout_glass_probe_grid,
        [RenderPipelineType_GlassProbePlane] = &layout_glass_probe_plane,
        [RenderPipelineType_Reflection] = &layout_reflection,
};

static const ComputePipelineStateObject
    *standard_compute_layouts[COMPUTE_PIPELINE_TYPE_COUNT] = {
        [ComputePipelineType_Mipmap] = &layout_mipmap,
        [ComputePipelineType_Kawase] = &layout_kawase,
};

/**
   Initialize standards shaders and build pipelines layout for each of them.

                 [ PSO ] ===> [ WGPUPipeline ]

 */
void standard_render_pipelines_init(
    const WGPUDevice device, const RenderPipelineMultisampleCount multisample) {

  VERBOSE_PROCESS("Initializing Standard Render Pipelines...");

  for (RenderPipelineType i = 0; i < RENDER_PIPELINE_TYPE_COUNT; i++) {

    const RenderPipelineStateObject *layout = standard_render_layouts[i];
    RenderPipeline *cached_pipeline = &g_std_render_pipelines[i];

    // create pipeline
    render_pipeline_create(cached_pipeline, &(RenderPipelineCreateDescriptor){
                                                .device = device,
                                                .label = layout->label,
                                                .path = layout->shader_path,
                                                .pso = layout,
                                            });

    {
      /* ===  CHECK CUSTOM ATTRIBUTES (weak check) === */
      // vertex state
      if (layout->pipeline_attributes.vertex_state.module != NULL)
        render_pipeline_set_vertex(cached_pipeline,
                                   layout->pipeline_attributes.vertex_state);

      // fragment state
      if (layout->pipeline_attributes.fragment_state.entryPoint != NULL)
        render_pipeline_set_fragment(
            cached_pipeline, &layout->pipeline_attributes.fragment_state);

      // primitive state
      if (layout->pipeline_attributes.primitive_state.cullMode !=
          WGPUCullMode_Undefined)
        render_pipeline_set_primitive(
            cached_pipeline, layout->pipeline_attributes.primitive_state);

      // stencil state
      if (layout->pipeline_attributes.stencil_state.format !=
          WGPUTextureFormat_Undefined)
        render_pipeline_set_stencil(cached_pipeline,
                                    layout->pipeline_attributes.stencil_state);

      // color state
      if (layout->pipeline_attributes.color_state.format !=
          WGPUTextureFormat_Undefined)
        render_pipeline_set_color(cached_pipeline,
                                  &layout->pipeline_attributes.color_state);

      // blend state
      if (layout->pipeline_attributes.blend_state.alpha.dstFactor)
        render_pipeline_set_blend(cached_pipeline,
                                  &layout->pipeline_attributes.blend_state);

      // if sampling set in custom attbutes, apply the config one
      if (layout->pipeline_attributes.multisample_state.count !=
          PipelineMultisampleCount_Undefined) {
        render_pipeline_set_multisample(
            cached_pipeline, &layout->pipeline_attributes.multisample_state);
      } else {
        // else use the renderer one
        render_pipeline_set_sampling(cached_pipeline, multisample);
      }
    }

    // build layout based on bindgroup description
    WGPUPipelineLayout temp_layout = shader_pipeline_state_object_create(
        layout->bind_groups, layout->bind_groups_count, device, NULL);

    render_pipeline_build(cached_pipeline, &temp_layout);
  }
}

void standard_compute_pipelines_init(const WGPUDevice device) {

  VERBOSE_PROCESS("Initializing Standard Compute Pipelines...");

  for (ComputePipelineType i = 0; i < COMPUTE_PIPELINE_TYPE_COUNT; i++) {

    const ComputePipelineStateObject *layout = standard_compute_layouts[i];
    ComputePipeline *cached_pipeline = &g_std_compute_pipelines[i];

    // create pipeline
    compute_pipeline_create(cached_pipeline, &(ComputePipelineCreateDescriptor){
                                                 .device = device,
                                                 .label = layout->label,
                                                 .path = layout->shader_path,
                                                 .pso = layout,
                                             });

    // build layout based on bindgroup description
    WGPUPipelineLayout temp_layout = shader_pipeline_state_object_create(
        layout->bind_groups, layout->bind_groups_count, device, NULL);

    compute_pipeline_build(cached_pipeline, &temp_layout);
  }
}

/**
   Transforms bindgroups into pipeline layouts and returns the generated
   pipeline descriptor
 */
WGPUPipelineLayout shader_pipeline_state_object_create(
    const WGPUBindGroupLayoutDescriptor *const *bind_groups, const size_t count,
    const WGPUDevice device, WGPUBindGroupLayout *outLayout) {

  const size_t layout_size = sizeof(WGPUBindGroupLayout) * count;

  WGPUBindGroupLayout *layouts = malloc(layout_size);

  for (size_t i = 0; i < count; i++)
    layouts[i] = wgpuDeviceCreateBindGroupLayout(device, bind_groups[i]);

  if (outLayout != NULL)
    memcpy(outLayout, layouts, layout_size);

  WGPUPipelineLayout pipeline_layout =
      wgpuDeviceCreatePipelineLayout(device, &(WGPUPipelineLayoutDescriptor){
                                                 .bindGroupLayoutCount = count,
                                                 .bindGroupLayouts = layouts,
                                             });

  free(layouts);
  layouts = NULL;

  return pipeline_layout;
}
