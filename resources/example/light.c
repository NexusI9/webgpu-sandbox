#include "light.h"

#include <stddef.h>

#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/systems/scene_system.h"

void example_light(Scene *scene, Renderer *renderer) {

  scene_system_add_point_light(scene, renderer,
                               &(PointLightDescriptor){
                                   .color = {0.4f, 0.0f, 1.0f, 1.0f},
                                   .intensity = 7.0f,
                                   .cutoff = 20.0f,
                                   .inner_cutoff = 50.0f,
                                   .near = 0.1,
                                   .far = 20.0f,
                                   .position = {0.0f, 3.5f, 0.0f},
                               },
                               LightCreateFlag_None, NULL);

  scene_system_add_sun_light(scene, renderer,
                             &(SunLightDescriptor){
                                 .position = {15.0f, 15.0f, 15.0f},
                                 .color = {0.0f, 0.4f, 1.0f, 1.0f},
                                 .intensity = 2.0f,
                                 .size = 10.0f,
                             },
                             LightCreateFlag_Shadow, NULL);

  /*
    scene_system_add_spot_light(scene,
    renderer
                         &(SpotLightDescriptor){
                             .color = {1.0f, 1.0f, 1.0f, 1.0f},
                             .intensity = 2.0f,
                             .cutoff = 45.0f,
                             .angle = 90.0f,
                             .inner_cutoff = 30.0f,
                             .target = {0.0f, 0.0f, 0.0f},
                             .position = {3.0f, 4.0f, -4.0f},
                         },
                         LightShadow_Enabled, NULL);
  */
  scene_system_add_ambient_light(scene, renderer,
                                 &(AmbientLightDescriptor){
                                     .color = {0.0f, 0.4f, 1.0f, 1.0f},
                                     .intensity = 0.2f,
                                     .position = {-2.0f, 3.0f, 3.3f},
                                 },
                                 NULL);
}
