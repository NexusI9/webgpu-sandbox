#include "light.h"

#include <stddef.h>

#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"

void example_light(Scene *scene) {

  scene_add_point_light(scene,
                        &(PointLightDescriptor){
                            .color = {0.4f, 0.0f, 1.0f},
                            .intensity = 7.0f,
                            .cutoff = 20.0f,
                            .inner_cutoff = 50.0f,
                            .near = 0.1,
                            .far = 20.0f,
                            .position = {0.0f, 3.5f, 0.0f},
                        },
                        LightShadow_None, NULL);

  scene_add_sun_light(scene,
                      &(SunLightDescriptor){
                          .position = {15.0f, 15.0f, 15.0f},
                          .color = {0.0f, 0.4f, 1.0f},
                          .intensity = 2.0f,
                          .size = 10.0f,
                      },
                      LightShadow_Enabled, NULL);

  scene_add_spot_light(scene,
                       &(SpotLightDescriptor){
                           .color = {1.0f, 1.0f, 1.0f},
                           .intensity = 2.0f,
                           .cutoff = 45.0f,
                           .angle = 90.0f,
                           .inner_cutoff = 30.0f,
                           .target = {0.0f, 0.0f, 0.0f},
                           .position = {3.0f, 4.0f, -4.0f},
                       },
                       LightShadow_Enabled, NULL);

  scene_add_ambient_light(scene,
                          &(AmbientLightDescriptor){
                              .color = {1.0f, 1.0f, 1.0f},
                              .intensity = 0.2f,
                              .position = {-2.0f, 1.0f, 0.3f},
                          },
                          NULL);
}
