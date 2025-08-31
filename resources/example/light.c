#include "light.h"

void example_light(Scene *scene) {

  scene_add_point_light(scene,
                        &(PointLightDescriptor){
                            .color = {1.0f, 0.0f, 0.3f},
                            .intensity = 4.0f,
                            .cutoff = 20.0f,
                            .inner_cutoff = 50.0f,
                            .near = 0.1,
                            .far = 20.0f,
                            .position = {0.0f, 2.4f, 2.3f},
                        },
                        LightShadow_Enabled);

  scene_add_sun_light(scene,
                      &(SunLightDescriptor){
                          .position = {-2.0f, 2.0f, 2.0f},
                          .color = {1.0f, 1.0f, 1.0f},
                          .intensity = 2.0f,
                          .size = 10.0f,
                      },
                      LightShadow_Enabled);

  scene_add_spot_light(scene,
                       &(SpotLightDescriptor){
                           .color = {1.0f, 0.0f, 0.4f},
                           .intensity = 2.0f,
                           .cutoff = 45.0f,
                           .angle = 90.0f,
                           .inner_cutoff = 30.0f,
                           .target = {0.0f, 0.0f, 0.0f},
                           .position = {3.0f, 4.0f, -4.0f},
                       },
                       LightShadow_Enabled);

  scene_add_ambient_light(scene, &(AmbientLightDescriptor){
                                     .color = {1.0f, 1.0f, 1.0f},
                                     .intensity = 0.2f,
                                     .position = {-2.0f, 1.0f, 0.3f},
                                 });
}
