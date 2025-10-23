#include "skybox.h"

#include <stdint.h>

#include "runtime/prefab/environment/skybox.h"
#include "runtime/texture/core.h"
#include "runtime/texture/write.h"

static const CubeMapPath lake_cubemap = {
    .right = "./resources/assets/texture/skybox/lake/right.jpg",
    .left = "./resources/assets/texture/skybox/lake/left.jpg",
    .top = "./resources/assets/texture/skybox/lake/top.jpg",
    .bottom = "./resources/assets/texture/skybox/lake/bottom.jpg",
    .front = "./resources/assets/texture/skybox/lake/front.jpg",
    .back = "./resources/assets/texture/skybox/lake/back.jpg",
};

static const CubeMapPath netherworld_cubemap = {
    .right = "./resources/assets/texture/skybox/netherworld/right.jpg",
    .left = "./resources/assets/texture/skybox/netherworld/left.jpg",
    .top = "./resources/assets/texture/skybox/netherworld/top.jpg",
    .bottom = "./resources/assets/texture/skybox/netherworld/bottom.jpg",
    .front = "./resources/assets/texture/skybox/netherworld/front.jpg",
    .back = "./resources/assets/texture/skybox/netherworld/back.jpg",
};

/**
   Create a picture based skybox in the given scene
 */
void example_skybox(Scene *scene) {
  prefab_skybox_create(
      scene,
      &(PrefabSkyboxCreateDescriptor){
          .blur = 4,
          .resolution = TextureResolution_512,
          .path = lake_cubemap,
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
