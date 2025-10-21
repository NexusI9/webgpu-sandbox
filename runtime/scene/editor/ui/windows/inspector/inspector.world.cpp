#include "inspector.world.hpp"
#include "imgui/imgui.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/components/tree_item.hpp"
#include "runtime/shader/update.h"
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

    // wgpuTextureGetWidth();

    ImGui::TreePop();
  }

  if (UI::TreeItem(scene, "Reflection").draw()) {

    const int planar_width =
        wgpuTextureGetWidth(scene->planes_reflection.pass.color.texture);
    const int planar_height =
        wgpuTextureGetHeight(scene->planes_reflection.pass.color.texture);

    name_t default_value;
    name_compose(default_value, "%d x %d", planar_width, planar_height);

    UI::Combobox planar_res =
        UI::Combobox(scene, "Plane resolution", &style, default_value);

    if (planar_res.draw()) {

      for (int i = 0; i < fixed_resolution_count; ++i) {

        const bool is_selected =
            (fixed_resolutions[i].resolution == planar_width);

        if (ImGui::Selectable(fixed_resolutions[i].label, is_selected)) {

          probe_reflection_list_update_resolution(
              &scene->planes_reflection.pass, fixed_resolutions[i].resolution,
              WGPUTextureViewDimension_2DArray);

          // update each lit mesh reflection view
          MeshRefList *reflection_meshes[SCENE_PIPELINE_REFLECTION_COUNT];
          scene_reflection_pipeline_meshes(scene, reflection_meshes);
          for (size_t i = 0; i < SCENE_PIPELINE_REFLECTION_COUNT; i++) {
            for (size_t j = 0; j < reflection_meshes[i]->length; j++) {

              Mesh *mesh = reflection_meshes[i]->entries[j];
              Shader *shader = mesh_shader(mesh, MeshShader_Texture);
              const PipelineBindingProbe *binding =
                  shader->pipeline->bindings.probe;

              shader_update_texture_view(
                  shader, binding->group, binding->reflection_plane_texture,
                  scene->planes_reflection.pass.color.attachment.view,
                  TEXTURE_FORMAT_OFFSCREEN, ShaderUpdateFlag_None);
            }
          }
        }

        // Set the initial focus when opening the combo (for keyboard
        // navigation)
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }

      planar_res.end();
    }

    ImGui::TreePop();
  }
}
