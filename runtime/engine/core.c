#include "core.h"
#include "backend/logger.h"
#include "backend/renderer/core.h"
#include "backend/renderer/reflection/core.h"
#include "backend/renderer/reflection/draw.h"
#include "backend/renderer/shadow_map/draw.h"
#include "backend/resource_manager.h"
#include "runtime/engine/add.h"
#include "runtime/light/list.h"
#include "runtime/scene/add.h"
#include "runtime/systems/scene_system.h"
#include "runtime/systems/selection_system.h"
#include "runtime/systems/ubo_system.h"
#include <stdint.h>

static inline void engine_init_shadow_map(LightList *, Renderer *);
static inline void engine_init_reflection_pass(ProbeList *, Renderer *);
static inline void engine_init_gizmo(Engine *, Gizmo *);

/**
   Initialize the engine with default values
 */
EngineStatus engine_init(Engine *engine) {

  engine->renderer = rem_new_renderer();
  renderer_create(engine_get_renderer(engine),
                  &(RendererCreateDescriptor){
                      .background = (WGPUColor){0.14f, 0.14f, 0.14f, 1.0f},
                      .dpi = 1.0,
                  });

  UBOManager *ubo = rem_new_ubo();
  ubo_init(ubo);
  ubo_system_register_draw_callback(ubo, engine_get_renderer(engine));

  Scene *scene = engine_add_scene(engine, &(SceneCreateDescriptor){
                                              .ubo = ubo,
                                              .viewport =
                                                  &(ViewportCreateDescriptor){
                                                      .fov = 32.0f,
                                                      .near_clip = 0.1f,
                                                      .far_clip = 100.0f,
                                                  },
                                          });
  engine_set_active_scene(engine, scene);

  engine->gui = rem_new_gui();
  gui_init(engine_get_gui(engine), &(GUIDescriptor){
                                       .active_scene = scene,
                                       .renderer = engine_get_renderer(engine),
                                       .theme = &g_theme,
                                       .dpi = g_context.dpi,
                                   });

  scene_system_create_grid(scene, engine_get_renderer(engine));
  engine_scene_add_mesh_pipeline(engine, scene->grid, RendererPipeline_Fixed,
                                 NULL, EngineAddFlag_Unselectable);

  selection_system_init(&scene->selection, scene, engine_get_renderer(engine));

  engine_init_shadow_map(&scene->lights, engine_get_renderer(engine));
  engine_init_reflection_pass(&scene->probes, engine_get_renderer(engine));

  engine_init_gizmo(engine, &scene->gizmo);
  scene_system_set_draw_mode(scene, engine_get_renderer(engine),
                             RendererDrawMode_Solid);

  return EngineStatus_Success;
}

EngineStatus engine_set_active_scene(Engine *engine, Scene *scene) {

  for (uint8_t i = 0; i < engine->scenes.length; i++) {
    if (engine->scenes.entries[i] == scene) {
      engine->scenes.active = scene;
      return EngineStatus_Success;
    }
  }

  logger_add(LoggerFlag_Warning, "Attempting to set active a scene that does "
                                 "not belong to the targeted engine.");

  return EngineStatus_UnfoundEntity;
}

void engine_init_reflection_pass(ProbeList *list, Renderer *renderer) {

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

void engine_init_shadow_map(LightList *list, Renderer *renderer) {

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

// Create transform gizmos and add them to editor gizmo
void engine_init_gizmo(Engine *engine, Gizmo *gizmo) {

  Scene *scene = engine_get_active_scene(engine);

  gizmo_create(gizmo, &(GizmoCreateDescriptor){
                          .camera = scene->active_camera,
                          .viewport = &scene->viewport,
                      });

  for (size_t i = 0; i < GIZMO_MODE_COUNT; i++) {
    for (size_t j = 0; j < gizmo->handles[i].length; j++) {
      Mesh *mesh = gizmo->handles[i].entries[j];
      engine_scene_add_mesh_pipeline(
          engine, mesh, RendererPipeline_Fixed_Front, SCENE_LAYER_GIZMO,
          EngineAddFlag_Hide | EngineAddFlag_Unselectable |
              EngineAddFlag_TreeHide);
    }
  }
}
