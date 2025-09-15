#include "skybox.h"

#include <stdint.h>

#include "../../runtime/prefab/environment/skybox.h"
#include "../runtime/texture/core.h"
#include "../runtime/texture/write.h"

/**
   Create a picture based skybox in the given scene
 */
void example_skybox(Scene *scene) {
  prefab_skybox_create(
      scene,
      &(PrefabSkyboxCreateDescriptor){
          .blur = 0.0f,
          .resolution = TextureResolution_512,
          .path =
              {
                  .right = "./resources/assets/texture/skybox/lake/right.jpg",
                  .left = "./resources/assets/texture/skybox/lake/left.jpg",
                  .top = "./resources/assets/texture/skybox/lake/top.jpg",
                  .bottom = "./resources/assets/texture/skybox/lake/bottom.jpg",
                  .front = "./resources/assets/texture/skybox/lake/front.jpg",
                  .back = "./resources/assets/texture/skybox/lake/back.jpg",
              },
      });
}

/**
   Create a gradient skybox in the given scene
 */
void example_skybox_gradient(Scene *scene) {

  prefab_skybox_gradient_create(
      scene, &(PrefabSkyboxGradientCreateDescriptor){
                 .resolution = 32,
                 .stops =
                     {
                         .length = 2,
                         .capacity = 2,
                         .entries =
                             (TextureGradientStop[]){
                                 {
                                     .color = (uint8_t[]){240, 245, 255, 255},
                                     .position = 1.0f,
                                 },
                                 {
                                     .color = (uint8_t[]){51, 153, 255, 255},
                                     .position = 0.0f,
                                 },
                             },
                     },
             });
}
