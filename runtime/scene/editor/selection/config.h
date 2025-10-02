#ifndef _SCENE_SELECTION_CONFIG_H_
#define _SCENE_SELECTION_CONFIG_H_

#include "runtime/scene/core.h"
#include "./callback/highlight.h"
#include "./callback/transform.h"

static inline void scene_selection_config(Scene *scene) {

  SceneLayer *exclude_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_UNSELECTABLE);

  SceneSelection *selection = &scene->editor.selection;

  // define selection config
  scene->editor.selection = (SceneSelection){
      // filters rules
      .filters =
          {
              // mesh based rule
              [SceneSelectionType_Mesh] =
                  {
                      .transform_callback = scene_selection_mesh_transform,
                      .highlight_callback = scene_selection_mesh_highlight,
                      .highlight_data = (void *)scene,
                  },
              // mesh shadow based rule
              [SceneSelectionType_MeshShadow] =
                  {
                      .transform_callback =
                          scene_selection_mesh_shadow_transform,
                      .highlight_callback = scene_selection_mesh_highlight,
                      .highlight_data = (void *)scene,
                  },
              // shader based rule
              [SceneSelectionType_SEM] =
                  {
                      .transform_callback = scene_selection_sem_transform,
                      .highlight_callback = scene_selection_sem_highlight,
                      .highlight_data = (void *)scene,
                  },

          }

  };
}

#endif
