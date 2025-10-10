#include "inspector.hpp"
#include "backend/registry.h"
#include "backend/std_pipeline/core.h"
#include "imgui/imgui.h"
#include "runtime/light/core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/ui/components/button_icon.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.ambient_light.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.mesh.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.point_light.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.spot_light.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.sun_light.hpp"
#include <cstdio>

static int g_active_tab = 0;
static RegEntry const *g_active_object = NULL;

static RenderPipelineMultisampleCount multisample_count[2] = {
    PipelineMultisampleCount_1x,
    PipelineMultisampleCount_4x,
};

void UI::SceneTab::draw() {

  ImGui::BeginChild("##scene_properties", ImVec2(0, 0), true,
                    ImGuiWindowFlags_NoScrollWithMouse);

  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                      ImVec2(ui->size[SceneEditorUISize_Tree_PaddingH],
                             ui->size[SceneEditorUISize_Tree_PaddingV]));
  // resolution / multisample
  if (ImGui::TreeNodeEx("Resolution", flags)) {
    {
      ImGui::Text("Width");
      width = scene_renderer_width(&scene->renderer);
      if (ImGui::DragInt("##width", &width)) {
        // update renderer
        scene_renderer_set_width(&scene->renderer, width);
        // update viewport + uniform
        {
          viewport_set_width(&scene->viewport, width);
          viewport_uniform_update(&scene->viewport);
          ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                   SSBOType_Viewport,
                                   ssbo_slot_id(&scene->viewport.ssbo_slot));
        }
        // update scene render texture
        scene_update_render_pass_texture(scene, width, height, multisample,
                                         dpi);
      }
    }

    {
      ImGui::Spacing();
      ImGui::Text("Height");
      height = scene_renderer_height(&scene->renderer);
      if (ImGui::DragInt("##height", &height)) {
        // update renderer
        scene_renderer_set_height(&scene->renderer, height);
        // update viewport + uniform
        {
          viewport_set_height(&scene->viewport, height);
          viewport_uniform_update(&scene->viewport);
          ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                   SSBOType_Viewport,
                                   ssbo_slot_id(&scene->viewport.ssbo_slot));
        }
        // update scene render texture
        scene_update_render_pass_texture(scene, width, height, multisample,
                                         dpi);
      }
    }

    {
      ImGui::Spacing();
      ImGui::Text("Multisample");

      char default_value[12];
      snprintf(default_value, 12, "x%d", context_multisample());
      multisample = context_multisample();
      if (ImGui::BeginCombo("##Multisample", default_value)) {
        for (int i = 0; i < IM_ARRAYSIZE(multisample_count); ++i) {

          const bool is_selected = (multisample_count[i] == multisample);

          char value[12];
          snprintf(value, 12, "x%d", multisample_count[i]);
          if (ImGui::Selectable(value, is_selected)) {
            RenderPipelineMultisampleCount count = multisample_count[i];
            context_set_multisample(count);

            // rebuild pipelines
            standard_render_pipelines_destroy();
            standard_render_pipelines_init(count);

            // update scene render texture
            scene_update_render_pass_texture(scene, width, height, count, dpi);

            // update passes relative draw callbacks for each modes
            for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++)
              render_pass_list_update_child_passes_callback(
                  &scene->renderer.draw.render_pass[i]);
          }

          // Set the initial focus when opening the combo (for keyboard
          // navigation)
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }

    {
      ImGui::Spacing();
      ImGui::Text("Device Pixel Ratio (DPI)");
      dpi = scene_renderer_dpi(&scene->renderer);
      if (ImGui::InputDouble("##dpi", &dpi)) {
        // update renderer
        scene_renderer_set_dpi(&scene->renderer, dpi);
        // update scene render texture
        scene_update_render_pass_texture(scene, width, height, multisample,
                                         dpi);
      }
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("View", flags)) {

    {
      ImGui::Text("FOV");
      fov = viewport_fov(&scene->viewport);
      if (ImGui::DragFloat("##FOV", &fov, 1, 10, 179.9)) {
        viewport_set_fov(&scene->viewport, fov);
        viewport_uniform_update(&scene->viewport);
        ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                 SSBOType_Viewport,
                                 ssbo_slot_id(&scene->viewport.ssbo_slot));
      }
    }

    {
      ImGui::Text("Near clip");
      near_clip = viewport_near_clip(&scene->viewport);
      if (ImGui::DragFloat("##Near Clip", &near_clip, 0.01, 0.01, 1000)) {
        viewport_set_near_clip(&scene->viewport, near_clip);
        viewport_uniform_update(&scene->viewport);
        ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                 SSBOType_Viewport,
                                 ssbo_slot_id(&scene->viewport.ssbo_slot));
      }
    }

    {
      ImGui::Text("Far clip");
      far_clip = viewport_far_clip(&scene->viewport);
      if (ImGui::DragFloat("##Far Clip", &far_clip, 0.01, 0.01, 1000)) {
        viewport_set_far_clip(&scene->viewport, far_clip);
        viewport_uniform_update(&scene->viewport);
        ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                 SSBOType_Viewport,
                                 ssbo_slot_id(&scene->viewport.ssbo_slot));
      }
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Bloom", flags)) {
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Vignette", flags)) {
    ImGui::TreePop();
  }

  ImGui::PopStyleVar(1);

  ImGui::EndChild();
}

void UI::SettingTab::draw() {}

void UI::ObjectTab::draw() {

  ImGui::BeginChild("##ObjectTab", ImVec2(0, 0), true);
  {

    reg_id_t target_id = set_active_target();
    g_active_object = reg_lookup(target_id); // O(1) so cheap in hot loop

    switch (g_active_object->type) {

    case RegEntryType_Mesh:
      InspectorMesh(scene, "Mesh properties", (Mesh *)g_active_object->ptr)
          .draw();
      break;

    case RegEntryType_PointLight:
      InspectorPointLight(scene, "Light properties",
                          (PointLight *)g_active_object->ptr)
          .draw();
      break;

    case RegEntryType_AmbientLight:
      InspectorAmbientLight(scene, "Light properties",
                            (AmbientLight *)g_active_object->ptr)
          .draw();
      break;

    case RegEntryType_SpotLight:
      InspectorSpotLight(scene, "Light properties",
                         (SpotLight *)g_active_object->ptr)
          .draw();
      break;

    case RegEntryType_SunLight:
      InspectorSunLight(scene, "Light properties",
                        (SunLight *)g_active_object->ptr)
          .draw();
      break;

    default:
      break;
    }
  }
  ImGui::EndChild();
}

bool UI::ObjectTab::is_valid_type(const RegEntryType type) {

  for (uint8_t i = 0; i < valid_type_len; i++)
    if (valid_type[i] == type)
      return true;

  return false;
}

reg_id_t UI::ObjectTab::set_active_target() {

  if (scene_selection_length(&scene->editor.selection)) {
    // get selection 1st entry
    for (int i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
      SceneSelectionObjectList *selection_list =
          &scene->editor.selection.filters[i].selection;
      if (selection_list->length) {
        return selection_list->entries[0].targets[SSOTargetID_Default];
      }
    }
  }

  MeshRefList *meshes[SCENE_DYNAMIC_PIPELINE_COUNT];
  size_t count;
  scene_dynamic_pipelines(scene, meshes, &count);
  for (uint8_t i = 0; i < count; i++)
    for (size_t j = 0; j < meshes[i]->length; j++)
      return meshes[i]->entries[j]->id;

  // TODO Make fallback id more robust
  return REG_OWNER_UNDEFINED;
}

void UI::Inspector::draw() {

  const float page_width = ui->size[SceneEditorUISize_RightPanel_Width];
  const float bar_width = ui->size[SceneEditorUISize_RightPanelTab_Width];
  const float gap = 4.f * ui->dpi;
  const float btn_size = ui->size[SceneEditorUISize_Button_RenderModeSize];
  const ImVec2 icon_size{btn_size, btn_size};

  ImGui::BeginChild("##Properties", ImVec2(0, 0), true);
  {
    ImGui::Separator();
    ImGui::Text("%s", label);
    ImGui::Spacing();
    {
      ImGui::BeginChild("##Properties Content", ImVec2(0, 0), true);
      {
        // tighten horizontal spacing
        ImGuiStyle &style = ImGui::GetStyle();
        float savedSpacingX = style.ItemSpacing.x;
        style.ItemSpacing.x = gap;

        // always-visible icon strip
        ImGui::BeginChild("##icon_bar", ImVec2(bar_width, 0), false,
                          ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoScrollWithMouse);
        {
          for (int i = 0; i < tab_count; ++i) {

            ImGui::PushID(i);
            bool sel = (g_active_tab == i);

            // highlight selected tab
            if (sel) {
              ImGui::PushStyleColor(ImGuiCol_Button,
                                    ImVec4(0.30f, 0.44f, 0.60f, 1.f));
              ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                    ImVec4(0.32f, 0.49f, 0.68f, 1.f));
              ImGui::PushStyleColor(ImGuiCol_Border,
                                    ImVec4(0.80f, 0.80f, 0.90f, 1.f));
            }

            bool pressed =
                ButtonIcon(ui, tabs[i]->icon, tabs[i]->tooltip, icon_size)
                    .draw();

            if (pressed)
              g_active_tab = i;

            if (ImGui::IsItemHovered() && strlen(tabs[i]->tooltip))
              ImGui::SetTooltip("%s", tabs[i]->tooltip);

            if (sel)
              ImGui::PopStyleColor(3);

            ImGui::PopID();
          }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // draw content
        if (g_active_tab >= 0) {
          ImGui::BeginChild("##page", ImVec2(page_width, 0), true,
                            ImGuiWindowFlags_NoScrollWithMouse);

          // header
          {
            ImGui::Text("%s", tabs[g_active_tab]->tooltip);
          }

          tabs[g_active_tab]->draw();

          ImGui::EndChild();
        }

        style.ItemSpacing.x = savedSpacingX;
      }
      ImGui::EndChild();
    }
    ImGui::EndChild();
  }
}
