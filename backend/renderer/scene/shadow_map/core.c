#include "core.h"
#include "../runtime/prefab/debug/view.h"
#include "../runtime/scene/draw.h"
#include "../runtime/texture/texture.h"
#include "../utils/math.h"
#include "../utils/system.h"
#include "./draw.h"
#include "./texture.h"
#include <string.h>

// static DebugView debug_view_light;
/**
                             For each shadow light:

               .-----------------.           .-----------------.
               |  Color Texture  |           |  Depth Texture  |
               '-----------------'           '-----------------'
                       |                              |
               .=======|====== RENDER LIGHT POV ======|=======.
               |       |                              |       |
               |       |     .----- vertex ----.      |       |
               |       |     |    Depth Pass   | <----|       |
               |       |     '-----------------'      |       |
               |       |              |               |       |
               |       |     .--- fragment ---.       |       |
               |       |---> |   Color Pass   |       |       |
               |       |     '----------------'       |       |
               |       |                              |       |
               '=======|==============================|======='
                       |_____________.    .___________|
                                     |   |
                            .---------------------.
                            | Light Texture Array |
                            '---------------------'



      Spot and Sun lights are all together stacked up in the same "directional
      light" texture array.
      Compared to the point lights that have a dedicated
      cubemap uniform entry.

      Another semantic to note is that Spot Lights and Sun Lights are both
      encompassed under the "Directional Light" term.

      The order is the following:

      .--------------. -----.
      |   Layer 0    |      |
      |--------------|      |
      |   Layer 1    |      |     .---------------.
      |--------------|       >----|  Spot Lights  | --.
      |   Layer 2    |      |     '--------------'    |
      |--------------|      |                         |
      |   Layer 3    | _____'                         |    .-------------------.
      |--------------| -----.                          >--| Directional Lights |
      |   Layer 4    |      |                         |   '--------------------'
      |--------------|      |     .--------------.    |
      |   Layer 5    |       >----|  Sun Lights  |----'
      |--------------|      |     '--------------'
      |   Layer 6    |      |
      '--------------' -----'


   */

void shadow_pass_init(const ShadowMapInitDescriptor *desc) {

  VERBOSE_PROCESS("Computing shadow map...");

  /*debug_view_create(&debug_view_light, &(DebugViewCreateDescriptor){
                                           .device = &device,
                                           .queue = &queue,
                                           });*/

  // create multi layered light texture (passed to the renderpass)
  size_t point_light_length = desc->lights.point->length;
  size_t spot_light_length = desc->lights.spot->length;
  size_t sun_light_length = desc->lights.spot->length;

  // Setup point light
  shadow_pass_texture_create(&(ShadowPassTextureDescriptor){
      .dimension = WGPUTextureViewDimension_CubeArray, // Cube array
      .layer_count = MAX(point_light_length, 1) * LIGHT_POINT_VIEWS,
      .device = *desc->device,
      .width = SHADOW_MAP_SIZE,
      .height = SHADOW_MAP_SIZE,
      .color =
          {
              .texture = &desc->lights.point->color_map,
              .texture_view = &desc->lights.point->color_view,
          },
      .depth =
          {
              .texture = &desc->lights.point->depth_map,
              .texture_view = &desc->lights.point->depth_view,
          },
  });

  // Setup directional lights

  shadow_pass_texture_create(&(ShadowPassTextureDescriptor){
      .dimension = WGPUTextureViewDimension_2DArray, // 2D Array
      .layer_count = MAX(spot_light_length + sun_light_length, 1),
      .device = *desc->device,
      .width = SHADOW_MAP_SIZE,
      .height = SHADOW_MAP_SIZE,
      .color =
          {
              .texture = &desc->lights.spot->color_map,
              .texture_view = &desc->lights.spot->color_view,
          },
      .depth =
          {
              .texture = &desc->lights.spot->depth_map,
              .texture_view = &desc->lights.spot->depth_view,
          },
  });

  // Generate Shadow maps (both color and depth map)
  shadow_map_draw_all(&(ShadowMapDrawAllDescriptor){
      .device = *desc->device,
      .queue = *desc->queue,
      .mesh_list = desc->mesh_list,
      .lights =
          {
              .point = desc->lights.point,
              .spot = desc->lights.spot,
              .sun = desc->lights.sun,
          },
      .point_light =
          {
              .color_texture = &desc->lights.point->color_map,
              .depth_texture = &desc->lights.point->depth_map,
          },
      .directional_light =
          {
              .color_texture = &desc->lights.spot->color_map,
              .depth_texture = &desc->lights.spot->depth_map,
          },
  });

  // !!DEBUG: Add views to scene
  /*for (size_t v = 0; v < debug_view_length(&debug_view_light); v++) {
    mesh *view = scene_new_mesh(scene, NULL);
    mesh *view_mesh = &debug_view_light.mesh[v];
    memcpy(view, view_mesh, sizeof(mesh));
    scene_add_mesh(scene, view, ScenePipeline_Dynamic_Unlit, NULL);
    }*/
}
