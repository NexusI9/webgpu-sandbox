#include "inspector.render.hpp"
#include "backend/postfx/core.h"
#include "backend/std_pipeline/core.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/components/tree_item.hpp"
#include "runtime/scene/renderer/core.h"
#include "runtime/scene/renderer/render_pass/core.h"

void UI::RenderTab::draw() {

  const InputStyle style = {
      .direction = InputDirection_Horizontal,
      .label_width = scene_editor_ui_size(ui, 85),
  };

  ImGui::BeginChild("##render_properties", ImVec2(0, 0), true,
                    ImGuiWindowFlags_NoScrollWithMouse);

  ImGui::PushItemWidth(-1);
  // resolution / multisample
  if (UI::TreeItem(scene, "Resolution").draw()) {

    {
      width = scene_renderer_width(&scene->renderer);
      if (UI::DragInt(scene, "Width", &style, &width, 1.0f, 1, 10000).draw()) {
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
      height = scene_renderer_height(&scene->renderer);
      if (UI::DragInt(scene, "Height", &style, &height, 1.0f, 1, 10000)
              .draw()) {
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

      char default_value[12];
      snprintf(default_value, 12, "x%d", context_multisample());

      UI::Combobox combobox =
          UI::Combobox(scene, "MSAA", &style, default_value);

      multisample = context_multisample();
      if (combobox.draw()) {
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
        combobox.end();
      }
    }

    {
      dpi = (float)scene_renderer_dpi(&scene->renderer);
      if (UI::DragFloat(scene, "DPI", &style, &dpi, 0.01f, 1, 4).draw()) {
        // update renderer
        scene_renderer_set_dpi(&scene->renderer, dpi);
        // update scene render texture
        scene_update_render_pass_texture(scene, width, height, multisample,
                                         dpi);
      }
    }
    ImGui::TreePop();
  }

  if (UI::TreeItem(scene, "View").draw()) {

    {
      fov = viewport_fov(&scene->viewport);
      if (UI::DragFloat(scene, "FOV", &style, &fov, 1.0f, 10.0f, 179.9f)
              .draw()) {
        viewport_set_fov(&scene->viewport, fov);
        viewport_uniform_update(&scene->viewport);
        ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                 SSBOType_Viewport,
                                 ssbo_slot_id(&scene->viewport.ssbo_slot));
      }
    }

    {
      near_clip = viewport_near_clip(&scene->viewport);
      if (UI::DragFloat(scene, "Near Clip", &style, &near_clip, 0.01f, 0.01f,
                        1000.0f)
              .draw()) {
        viewport_set_near_clip(&scene->viewport, near_clip);
        viewport_uniform_update(&scene->viewport);
        ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                 SSBOType_Viewport,
                                 ssbo_slot_id(&scene->viewport.ssbo_slot));
      }
    }

    {
      far_clip = viewport_far_clip(&scene->viewport);
      if (UI::DragFloat(scene, "Far Clip", &style, &far_clip, 0.01f, 0.01f,
                        1000.0f)
              .draw()) {

        viewport_set_far_clip(&scene->viewport, far_clip);
        viewport_uniform_update(&scene->viewport);
        ssbo_update_queue_insert(scene_renderer_ssbo(&scene->renderer),
                                 SSBOType_Viewport,
                                 ssbo_slot_id(&scene->viewport.ssbo_slot));
      }
    }
    ImGui::TreePop();
  }

  PostFx *texture_pass_fx =
      &render_pass_list_last_pass(
           scene_renderer_mode_pass_list(&scene->renderer,
                                         SceneRendererDrawMode_Texture))
           ->post_fx;

  PostFxEffect *bloom = post_fx_effect(texture_pass_fx, PostFxType_Bloom);
  PostFxEffect *composite =
      post_fx_effect(texture_pass_fx, PostFxType_Composite);

  if (UI::TreeItem(scene, "Bloom").draw()) {

    if (UI::DragFloat(scene, "Threshold", &style,
                      &bloom->uniform.bloom.threshold, 0.01f, 0.0f, 1.0f)
            .draw())
      post_fx_bloom_update_uniform(texture_pass_fx, bloom->uniform.bloom);

    if (UI::DragFloat(scene, "Knee", &style, &bloom->uniform.bloom.knee, 0.01f,
                      0.0f, 1.0f)
            .draw())
      post_fx_bloom_update_uniform(texture_pass_fx, bloom->uniform.bloom);

    if (UI::DragInt(scene, "Blur", &style, (int *)&bloom->uniform.bloom.blur,
                    1.0f, 0, 10)
            .draw())
      post_fx_bloom_update_uniform(texture_pass_fx, bloom->uniform.bloom);

    if (UI::DragFloat(scene, "Intensity", &style,
                      &composite->uniform.composite.bloom_intensity, 0.01f,
                      0.0f, 10.0f)
            .draw())
      post_fx_composite_update_uniform(texture_pass_fx,
                                       composite->uniform.composite);

    if (UI::DragInt(scene, "Downscale", &style,
                    (int *)&bloom->uniform.bloom.downscale, 1, 1, 6)
            .draw())
      post_fx_bloom_update_texture_resolution(texture_pass_fx, width, height);

    ImGui::TreePop();
  }

  if (UI::TreeItem(scene, "Vignette").draw()) {

    if (UI::DragFloat(scene, "Strength", &style,
                      &composite->uniform.composite.vignette_strength, 0.01f,
                      0.0f, 1.0f)
            .draw())
      post_fx_composite_update_uniform(texture_pass_fx,
                                       composite->uniform.composite);

    if (UI::DragFloat(scene, "Feather", &style,
                      &composite->uniform.composite.vignette_feather, 0.01f,
                      0.0f, 1.0f)
            .draw())
      post_fx_composite_update_uniform(texture_pass_fx,
                                       composite->uniform.composite);

    if (UI::DragFloat(scene, "Exposure", &style,
                      &composite->uniform.composite.exposure, 0.01f, 0.0f,
                      10.0f)
            .draw())
      post_fx_composite_update_uniform(texture_pass_fx,
                                       composite->uniform.composite);

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
  ImGui::EndChild();
}
