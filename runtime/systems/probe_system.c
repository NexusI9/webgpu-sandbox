#include "probe_system.h"
#include "backend/renderer/reflection/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/probe/reflection/probe.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"

void probe_system_init_reflection_pass(ProbeList *list, Renderer *renderer) {

  const RendererPipeline reflection_pipelines[2] = {
      RendererPipeline_Dynamic_LitShadow,
      RendererPipeline_Dynamic_Lit,
  };

  RenderPassDrawListDescriptor reflection_draw_list = {.length = 2};
  for (uint8_t i = 0; i < 2; i++)
    reflection_draw_list.entries[i] = (RenderPassDrawLayoutDescriptor){
        .shader = MeshShader_Reflection,
        .topology_callback = mesh_topology_base,
        .meshes = renderer_pipeline(renderer, reflection_pipelines[i]),
        .mesh_preprocessor_callback = probe_reflection_list_draw_preprocessor,
    };

  // grid reflection
  renderer_probe_reflection_create_pass(
      renderer, &(RendererProbeReflectionDescriptor){
                    .layer_count = PROBE_REFLECTION_LIST_MAX_COUNT *
                                   PROBE_REFLECTION_VIEW_COUNT,
                    .draw_list = &reflection_draw_list,
                    .handle = &list->reflection_probe.pass,
                    .multisample = PipelineMultisampleCount_1x,
                    .resolution = TextureResolution_512,
                    .view_dimension = WGPUTextureViewDimension_CubeArray,
                });

  // plane relfection
  renderer_probe_reflection_create_pass(
      renderer, &(RendererProbeReflectionDescriptor){
                    .layer_count = PROBE_REFLECTION_PLANE_LIST_LAYER_COUNT,
                    .draw_list = &reflection_draw_list,
                    .handle = &list->reflection_plane.pass,
                    .multisample = PipelineMultisampleCount_1x,
                    .resolution = TextureResolution_512,
                    .view_dimension = WGPUTextureViewDimension_2DArray,
                });
}
