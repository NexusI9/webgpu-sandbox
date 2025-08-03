#ifndef _SCENE_SELECTION_CONFIG_H_
#define _SCENE_SELECTION_CONFIG_H_

#include "../../core.h"
#include "./callback_transform.h"

static inline void scene_selection_config(Scene *scene) {

  SceneLayer *exclude_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_UNSELECTABLE);

  SceneSelection *selection = &scene->editor.selection;

  // define selection config
  scene->editor.selection = (SceneSelection){
      // main selection rule
      .include =
          {
              .entries =
                  {
                      &scene->pipelines[ScenePipeline_Dynamic_Lit],
                      &scene->pipelines[ScenePipeline_Dynamic_Unlit],
                      &scene->pipelines[ScenePipeline_Fixed],
                  },
              .length = 3,
          },
      .exclude =
          {
              .entries =
                  {
                      &exclude_layer->meshes,
                  },
              .length = 1,
          },
      // filters rules
      .filters =
          {
              // mesh based rule
              [SceneSelectionType_Mesh] =
                  {
                      .include =
                          {
                              .entries =
                                  {
                                      &scene->pipelines
                                           [ScenePipeline_Dynamic_Lit],
                                      &scene->pipelines
                                           [ScenePipeline_Dynamic_Unlit],
                                  },
                              .length = 2,
                          },
                      .exclude =
                          {
                              .entries =
                                  {
                                      &scene->pipelines[ScenePipeline_Fixed],
                                  },
                              .length = 1,
                          },
                      .transfert =
                          &scene->pipelines[ScenePipeline_Fixed_Selection],
                      .transform_callbacks =
                          {
                              [GizmoTransformMode_Translate] =
                                  scene_selection_mesh_translate,
                              [GizmoTransformMode_Rotate] =
                                  scene_selection_mesh_rotate,
                              [GizmoTransformMode_Scale] =
                                  scene_selection_mesh_scale,
                          },
                  },

              // shader based rule
              [SceneSelectionType_Shader] =
                  {
                      .include =
                          {
                              .entries =
                                  {
                                      &scene->pipelines[ScenePipeline_Fixed],
                                  },
                              .length = 1,
                          },
                      .exclude =
                          {
                              .entries =
                                  {
                                      &scene->pipelines
                                           [ScenePipeline_Dynamic_Lit],
                                      &scene->pipelines
                                           [ScenePipeline_Dynamic_Unlit],
                                  },
                              .length = 2,
                          },
                      .transfert = NULL,
                      .transform_callbacks =
                          {
                              [GizmoTransformMode_Translate] =
                                  scene_selection_shader_translate,
                              [GizmoTransformMode_Rotate] =
                                  scene_selection_shader_rotate,
                              [GizmoTransformMode_Scale] =
                                  scene_selection_shader_scale,
                          },
                  },

          }

  };
}

#endif
