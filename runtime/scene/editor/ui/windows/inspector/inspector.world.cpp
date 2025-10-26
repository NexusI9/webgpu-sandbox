#include "inspector.world.hpp"
#include "imgui/imgui.h"
#include "runtime/light/list.h"
#include "runtime/light/shadow_map/core.h"
#include "runtime/light/shadow_map/draw.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/ui/components/combobox_resolution.hpp"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/components/spacing.hpp"
#include "runtime/scene/editor/ui/components/tree_item.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "utils/name.h"
#include "webgpu/webgpu.h"

void UI::WorldTab::draw() {

  const InputStyle style = {
      .direction = InputDirection_Vertical,
      .label_width = scene_editor_ui_size(ui, 85),
  };

  if (UI::TreeItem(scene, "Skybox").draw()) {

    ImGui::TreePop();
  }

  if (UI::TreeItem(scene, "Shadow").draw()) {

    { // === Point Light ===
      const int width =
          wgpuTextureGetWidth(scene->lights.point.shadow.pass.color.texture);
      const int height =
          wgpuTextureGetHeight(scene->lights.point.shadow.pass.color.texture);

      name_t default_value;
      name_compose(default_value, "%d x %d", width, height);

      ImGui::PushID("point_light_res");
      UI::ComboboxResolution(scene, "Point light resolution", default_value,
                             &style, (TextureResolution)width,
                             on_resolution_change_point_light)
          .draw();
      ImGui::PopID();
    }

    UI::Spacing(ui, SceneEditorUISize_Space_Small).draw_y();

    { // === Directional Light ===
      const int width =
          wgpuTextureGetWidth(scene->lights.spot.shadow.pass.color.texture);
      const int height =
          wgpuTextureGetHeight(scene->lights.spot.shadow.pass.color.texture);

      name_t default_value;
      name_compose(default_value, "%d x %d", width, height);

      ImGui::PushID("dir_light_res");
      UI::ComboboxResolution(scene, "Dir light resolution", default_value,
                             &style, (TextureResolution)width,
                             on_resolution_change_dir_light)
          .draw();
      ImGui::PopID();
    }

    UI::Spacing(ui, SceneEditorUISize_Space_Small).draw_y();

    ImGui::TreePop();
  }

  if (UI::TreeItem(scene, "Reflection").draw()) {

    const int width =
        wgpuTextureGetWidth(scene->probes.reflection_plane.pass.color.texture);
    const int height =
        wgpuTextureGetHeight(scene->probes.reflection_plane.pass.color.texture);

    name_t default_value;
    name_compose(default_value, "%d x %d", width, height);

    UI::ComboboxResolution(scene, "Planar resolution", default_value, &style,
                           (TextureResolution)width,
                           on_resolution_change_plane_reflection)
        .draw();

    ImGui::TreePop();
  }
}

void UI::WorldTab::on_resolution_change_point_light(
    Scene *scene, const TextureResolution resolution) {

  // update each lit shadowed mesh view
  MeshRefList *shadowed_meshlist =
      scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow);

  RenderPass *pass = &scene->lights.point.shadow.pass;
  shadow_pass_update_resolution(pass, resolution,
                                WGPUTextureViewDimension_CubeArray);

  // update each mesh bound shadow view
  for (size_t j = 0; j < shadowed_meshlist->length; j++) {

    Mesh *mesh = shadowed_meshlist->entries[j];
    Shader *shader = mesh_shader(mesh, MeshShader_Texture);
    const PipelineBindingLightList *binding =
        shader->pipeline->bindings.light_list;

    shader_update_texture_view(shader, binding->group, binding->point_texture,
                               pass->depth.attachment.view, SHADOW_DEPTH_FORMAT,
                               ShaderUpdateFlag_None);
  }

  // redraw lights with new resolution
  size_t i;
  for (i = 0; i < scene->lights.point.shadow.length; i++) {
    ShadowMapDrawPointLightDescriptor desc = {
        .light = scene->lights.point.shadow.entries[i],
        .pass = pass,
        .texture_layer = i,
        .command_encoder = NULL,
        .profiler = &scene->renderer.profiler,
    };
    shadow_map_draw_point_light(&desc, SCENE_DEBUG_UNDEFINED);
  }
}

void UI::WorldTab::on_resolution_change_dir_light(
    Scene *scene, const TextureResolution resolution) {

  // update each lit shadowed mesh view
  MeshRefList *shadowed_meshlist =
      scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow);

  RenderPass *pass = &scene->lights.spot.shadow.pass;
  shadow_pass_update_resolution(pass, resolution,
                                WGPUTextureViewDimension_2DArray);

  // update each mesh bound shadow view
  for (size_t j = 0; j < shadowed_meshlist->length; j++) {

    Mesh *mesh = shadowed_meshlist->entries[j];
    Shader *shader = mesh_shader(mesh, MeshShader_Texture);
    const PipelineBindingLightList *binding =
        shader->pipeline->bindings.light_list;

    shader_update_texture_view(shader, binding->group,
                               binding->directional_texture,
                               pass->depth.attachment.view, SHADOW_DEPTH_FORMAT,
                               ShaderUpdateFlag_None);
  }

  // redraw lights with new resolution
  size_t i;
  for (i = 0; i < scene->lights.sun.shadow.length; i++) {
    ShadowMapDrawSunLightDescriptor desc = {
        .light = scene->lights.sun.shadow.entries[i],
        .pass = pass,
        .texture_layer = light_list_sun_layer_index(&scene->lights, i),
        .command_encoder = NULL,
        .profiler = &scene->renderer.profiler,
    };
    shadow_map_draw_sun_light(&desc, SCENE_DEBUG_UNDEFINED);
  }

  for (i = 0; i < scene->lights.spot.shadow.length; i++) {
    ShadowMapDrawSpotLightDescriptor desc = {
        .light = scene->lights.spot.shadow.entries[i],
        .pass = pass,
        .texture_layer = i,
        .command_encoder = NULL,
        .profiler = &scene->renderer.profiler,
    };
    shadow_map_draw_spot_light(&desc, SCENE_DEBUG_UNDEFINED);
  }
}

void UI::WorldTab::on_resolution_change_plane_reflection(
    Scene *scene, const TextureResolution resolution) {

  probe_reflection_list_update_resolution(&scene->probes.reflection_plane.pass,
                                          resolution,
                                          WGPUTextureViewDimension_2DArray);

  // update each lit mesh reflection view
  MeshRefList *reflection_meshes[SCENE_PIPELINE_REFLECTION_COUNT];
  scene_reflection_pipeline_meshes(scene, reflection_meshes);
  for (size_t j = 0; j < SCENE_PIPELINE_REFLECTION_COUNT; j++) {
    for (size_t k = 0; k < reflection_meshes[j]->length; k++) {

      Mesh *mesh = reflection_meshes[j]->entries[k];
      Shader *shader = mesh_shader(mesh, MeshShader_Texture);
      const PipelineBindingProbe *binding = shader->pipeline->bindings.probe;

      shader_update_texture_view(
          shader, binding->group, binding->reflection_plane_texture,
          scene->probes.reflection_plane.pass.color.attachment.view,
          TEXTURE_FORMAT_OFFSCREEN, ShaderUpdateFlag_None);
    }
  }
}
