#include "ao.h"

void example_ao(Engine *engine, bool debug) {

  AOBakeDrawDebug debug_options = {0};

  if (debug) {
    debug_options = (AOBakeDrawDebug){
        .debug_scene = &engine_get_active_scene(engine)->debug,
        .color = &(color){0.0f, 1.0f, 0.0f, 1.0f},
        .max_ray = 20,
    };
  }

  RendererBatchMeshLists lit_meshes;
  renderer_batch_get_mesh_list_with_flags(&engine_get_renderer(engine)->batches,
                                          RendererBatchFlag_Lit, &lit_meshes);

  for (size_t i = 0; i < lit_meshes.count; i++)
    ao_bake_draw_list(&engine_get_renderer(engine)->texture.ambient_occlusion,
                      &(AOBakeDrawDescriptor){
                          .mesh_list = lit_meshes.entries[i],
                          .global =
                              {
                                  AO_GLOBAL_RAY_AMOUNT,
                                  AO_GLOBAL_RAY_MAX_DISTANCE,
                              },
                          .local =
                              {
                                  AO_LOCAL_RAY_AMOUNT,
                                  AO_LOCAL_RAY_MAX_DISTANCE,
                              },
                          .debug = &debug_options,
                      });
}
