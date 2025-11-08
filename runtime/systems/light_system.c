#include "light_system.h"
#include "backend/renderer/shadow_map/core.h"
#include "runtime/light/list.h"

void light_system_init_shadow_map(LightList *list,
                                             Renderer *renderer) {

  shadow_map_init(&(ShadowMapInitDescriptor){
      .lights = list,
      .draw_list =
          &(RenderPassDrawListDescriptor){
              .length = 1,
              .entries =
                  {
                      {
                          .shader = MeshShader_Shadow,
                          .topology_callback = mesh_topology_base,
                          .mesh_preprocessor_callback =
                              shadow_map_pass_preprocessor_callback,
                          .mesh_preprocessor_data = (void *)NULL,
                          .meshes = renderer_pipeline(
                              renderer, RendererPipeline_Dynamic_LitShadow),
                      },
                  },
          },
  });
}
