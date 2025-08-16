#include "./utils.h"

void seo_light_update_shadow_map(Scene* scene) {

    // update lit and lit shadow meshes light uniforms
    const MeshRefList *pipelines[2] = {
        scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
        scene_pipeline(scene, ScenePipeline_Dynamic_Lit),
    };

    // traverse and update texture shader
    for (uint8_t i = 0; i < 2; i++) {
      for (size_t j = 0; j < pipelines[i]->length; j++) {
        Mesh *mesh = pipelines[i]->entries[j];
        Shader *shader = mesh_shader_texture(mesh);
        // TODO: only update point light uniforms
        mesh_shader_texture_update_lights(mesh, &scene->lights,
                                          SHADER_TEXTURE_BINDGROUP_LIGHTS);
      }
    }
  
}
