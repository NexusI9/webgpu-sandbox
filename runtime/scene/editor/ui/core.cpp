#include "core.h"
#include "./imgui_style/style.carbon.hpp"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/ssbo.h"
#include "backend/std_pipeline/core.h"
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_wgpu.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/html_event/add.h"
#include "runtime/html_event/core.h"
#include "runtime/input/core.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/core.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/selection/filter.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/editor/selection/utils.h"
#include "runtime/scene/editor/ui/windows/browser.hpp"
#include "runtime/scene/editor/ui/windows/display.hpp"
#include "runtime/scene/editor/ui/windows/gizmo.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"
#include "runtime/scene/editor/ui/windows/log.hpp"
#include "runtime/scene/editor/ui/windows/monitor.hpp"
#include "runtime/scene/editor/ui/windows/render_mode.hpp"
#include "runtime/scene/editor/ui/windows/tree.hpp"
#include "runtime/scene/renderer/core.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/scene/show.h"
#include "runtime/texture/atlas.h"
#include "runtime/texture/core.h"
#include "runtime/viewport/core.h"
#include "stdio.h"
#include <cstring>
#include <stdint.h>

/* TODO: make context available in the scene editor ui, but may interfere witht
 * the "pure C" approach since SceneEditorUI is included in Scene.
 */
static ImGuiContext *imgui_context;

// ui init
static inline void scene_editor_ui_set_icon_cell(SceneEditorUI *);
static inline void scene_editor_ui_set_size(SceneEditorUI *);
static inline void scene_editor_ui_create_texture(SceneEditorUI *);

// layouts
static inline void scene_editor_ui_create_top_bar(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_left_panel(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_right_panel(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_bottom_panel(SceneEditorUI *,
                                                       Scene *);

bool scene_editor_keydown_callback(int eventType,
                                   const EmscriptenKeyboardEvent *e,
                                   void *userData) {
  ImGuiIO &io = ImGui::GetIO();
  ImGuiKey key = (ImGuiKey)(ImGuiKey_NamedKey_BEGIN + e->keyCode);

  if (e->which > 0 && e->which < 0x10000)
    io.AddInputCharacter((unsigned int)e->which);

  return EM_TRUE;
}

SceneEditorUIStatus scene_editor_ui_init(SceneEditorUI *ui,
                                         const SceneEditorUIDescriptor *desc) {

  logger_add(LoggerFlag_Process, "Intitializing Editor UI");

  {
    ui->id = reg_register(ui, RegEntryType_SceneUI);
    ui->clock = desc->clock;
    ui->dpi = desc->dpi;
    scene_editor_ui_create_texture(ui);
    scene_editor_ui_set_size(ui);
    scene_editor_ui_set_icon_cell(ui);
    scene_editor_ui_tree_create(&ui->tree, SCENE_EDITOR_UI_TREE_CAPACITY);

    HTMLEventKey keydown_desc = {
        .callback = scene_editor_keydown_callback,
        .owner = ui->id,
    };
    html_event_add_key_down(&keydown_desc);
  }

  {
    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);
    scene_editor_ui_style_carbon();

    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(
        "./resources/assets/font/GolosText-Regular.ttf", 14.0f);

    ImGui_ImplWGPU_InitInfo info;

    info.Device = context_device();
    info.RenderTargetFormat = TEXTURE_FORMAT_ONSCREEN;
    info.DepthStencilFormat = TEXTURE_FORMAT_DEPTH;
    info.NumFramesInFlight = 3;

    ImGui_ImplWGPU_Init(&info);
  }

  return SceneEditorUIStatus_Success;
}


void scene_editor_ui_draw_callback(void *data) {
  Scene *scene = (Scene *)data;
  SceneEditorUI *ui = &scene->editor.ui;

  WGPUCommandEncoderDescriptor com_enc_desc = {.label = "Scene UI Command"};
  WGPUCommandEncoder command_encoder =
      wgpuDeviceCreateCommandEncoder(context_device(), &com_enc_desc);

  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(context_swapchain());

  WGPURenderPassColorAttachment color_attachment = {
      .view = swapchain_view,
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
      .resolveTarget = NULL,
      .loadOp = WGPULoadOp_Load,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
  };

  WGPURenderPassDepthStencilAttachment depth_attachment = {
      .view = ui->depth_view,
      .depthLoadOp = WGPULoadOp_Clear,
      .depthStoreOp = WGPUStoreOp_Store,
      .depthClearValue = 1.0f,
      .depthReadOnly = false,
  };

  WGPURenderPassDescriptor render_pass_desc = {
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
      .depthStencilAttachment = &depth_attachment,
  };

  ui->pass_encoder =
      wgpuCommandEncoderBeginRenderPass(command_encoder, &render_pass_desc);

  {
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize.x = (float)context_width() * ui->dpi;
    io.DisplaySize.y = (float)context_height() * ui->dpi;
    io.DeltaTime = ui->clock->delta;
    io.FontGlobalScale = ui->dpi;
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.MousePos = ImVec2(g_input.mouse.x * ui->dpi, g_input.mouse.y * ui->dpi);
    io.MouseDown[0] = g_input.mouse.state;
    io.MouseWheel = g_input.mouse.wheel.deltaY;
    io.MouseWheelH = g_input.mouse.wheel.deltaX;

    if (io.WantCaptureMouse)
      g_input.locked |= InputLockState_Mouse;
    else if (g_input.locked & InputLockState_Mouse)
      g_input.locked ^= InputLockState_Mouse;
  }

  {
    ImGui_ImplWGPU_NewFrame();
    ImGui::NewFrame();
    {

      if (UI::Display::state & UI::DisplayState_Layout) {
        scene_editor_ui_create_top_bar(ui, scene);
        scene_editor_ui_create_right_panel(ui, scene);
        scene_editor_ui_create_bottom_panel(ui, scene);
        UI::Gizmo(scene, "Gizmo").draw();
      }

      if (UI::Display::state & UI::DisplayState_Activity) {
        UI::Monitor(scene, "Monitor").draw();
      }

      UI::Display(scene, "Display").draw();
    }
    ImGui::Render();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), ui->pass_encoder);
  }

  {
    wgpuRenderPassEncoderEnd(ui->pass_encoder);
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(command_encoder, NULL);
    wgpuQueueSubmit(context_queue(), 1, &command_buffer);
    wgpuTextureViewRelease(swapchain_view);
  }
}

void scene_editor_ui_set_size(SceneEditorUI *ui) {

  static int base_size[SCENE_EDITOR_UI_SIZE_COUNT] = {
      [SceneEditorUISize_Screen_Width] = context_width(),
      [SceneEditorUISize_Screen_Height] = context_height(),
      [SceneEditorUISize_RightPanel_Width] = 250,
      [SceneEditorUISize_RightPanelTab_Width] = 30,
      [SceneEditorUISize_Tree_Height] = 400,
      [SceneEditorUISize_Tree_PaddingV] = 4,
      [SceneEditorUISize_Tree_PaddingH] = 1,
      [SceneEditorUISize_TopBar_Height] = 40,
      [SceneEditorUISize_TopBar_Margin] = 0,
      [SceneEditorUISize_Gizmo_Width] = 100,
      [SceneEditorUISize_Gizmo_Height] = 400,
      [SceneEditorUISize_Gizmo_Margin] = 10,
      [SceneEditorUISize_Button_RenderModeSize] = 22,
      [SceneEditorUISize_Button_InspectorTab] = 15,
      [SceneEditorUISize_Button_GizmoSize] = 35,
      [SceneEditorUISize_Button_DisplaySize] = 24,
      [SceneEditorUISize_BottomPanel_Height] = 200,
      [SceneEditorUISize_Log_IconScale] = 16,
      [SceneEditorUISize_Monitor_Width] = 300,
      [SceneEditorUISize_Monitor_Height] = 100,
  };

  for (uint16_t i = 0; i < SCENE_EDITOR_UI_SIZE_COUNT; i++)
    ui->size[i] = base_size[i] * (int)ui->dpi;
}

void scene_editor_ui_set_icon_cell(SceneEditorUI *ui) {

  // define icon position on atlas

  ui->icon_uv[SceneEditorUIIcon_Null] = {.cell = {15, 15}};

  {
    // render modes
    ui->icon_uv[SceneEditorUIIcon_RenderMode_Boundbox] = {.cell = {5, 0}};
    ui->icon_uv[SceneEditorUIIcon_RenderMode_Wireframe] = {.cell = {6, 0}};
    ui->icon_uv[SceneEditorUIIcon_RenderMode_Solid] = {.cell = {7, 0}};
    ui->icon_uv[SceneEditorUIIcon_RenderMode_Texture] = {.cell = {8, 0}};
  }

  {
    // gizmo
    ui->icon_uv[SceneEditorUIIcon_Gizmo_Position] = {.cell = {9, 0}};
    ui->icon_uv[SceneEditorUIIcon_Gizmo_Rotate] = {.cell = {10, 0}};
    ui->icon_uv[SceneEditorUIIcon_Gizmo_Scale] = {.cell = {11, 0}};
  }

  {
    // log
    ui->icon_uv[SceneEditorUIIcon_Log_Error] = {.cell = {12, 0}};
    ui->icon_uv[SceneEditorUIIcon_Log_Warning] = {.cell = {13, 0}};
    ui->icon_uv[SceneEditorUIIcon_Log_Info] = {.cell = {14, 0}};
    ui->icon_uv[SceneEditorUIIcon_Log_Success] = {.cell = {15, 0}};
    ui->icon_uv[SceneEditorUIIcon_Log_Import] = {.cell = {0, 1}};
    ui->icon_uv[SceneEditorUIIcon_Log_Process] = {.cell = {1, 1}};
  }

  {
    // bool
    ui->icon_uv[SceneEditorUIIcon_Layout] = {.cell = {2, 1}};
    ui->icon_uv[SceneEditorUIIcon_Activity] = {.cell = {3, 1}};
    ui->icon_uv[SceneEditorUIIcon_Eye] = {.cell = {4, 1}};
    ui->icon_uv[SceneEditorUIIcon_EyeOff] = {.cell = {5, 1}};
  }

  // prop tab
  {
    ui->icon_uv[SceneEditorUIIcon_Properties_Scene] = {.cell = {6, 1}};
    ui->icon_uv[SceneEditorUIIcon_Properties_Setting] = {.cell = {7, 1}};
    ui->icon_uv[SceneEditorUIIcon_Properties_Object] = {.cell = {8, 1}};
  }

  // solid
  {
    ui->icon_uv[SceneEditorUIIcon_PointLight] = {.cell = {0, 0}};
    ui->icon_uv[SceneEditorUIIcon_SunLight] = {.cell = {1, 0}};
    ui->icon_uv[SceneEditorUIIcon_SpotLight] = {.cell = {2, 0}};
    ui->icon_uv[SceneEditorUIIcon_AmbientLight] = {.cell = {3, 0}};
    ui->icon_uv[SceneEditorUIIcon_Mesh] = {.cell = {9, 1}};
    ui->icon_uv[SceneEditorUIIcon_ProbeReflectionPlane] = {.cell = {10, 1}};
    ui->icon_uv[SceneEditorUIIcon_ProbeReflectionGrid] = {.cell = {11, 1}};
    ui->icon_uv[SceneEditorUIIcon_Camera] = {.cell = {4, 0}};
    ui->icon_uv[SceneEditorUIIcon_Grid] = {.cell = {12, 1}};
  }

  // generate uvs
  for (uint8_t i = 0; i < SCENE_EDITOR_UI_ICON_COUNT; i++)
    texture_atlas_cell_uv(&ui->atlas_texture, ui->icon_uv[i].cell,
                          ui->icon_uv[i].uv0, ui->icon_uv[i].uv1);
}

void scene_editor_ui_create_texture(SceneEditorUI *ui) {

  // Depth texture
  {
    WGPUTextureDescriptor tex_desc = {
        .label = "Scene UI Depth Texture",
        .dimension = WGPUTextureDimension_2D,
        .size =
            {
                .width = (uint32_t)(context_width() * ui->dpi),
                .height = (uint32_t)(context_height() * ui->dpi),
                .depthOrArrayLayers = 1,
            },
        .mipLevelCount = 1,
        .sampleCount = 1,
        .format = TEXTURE_FORMAT_DEPTH,
        .usage = WGPUTextureUsage_RenderAttachment,
    };

    ui->depth_texture = wgpuDeviceCreateTexture(context_device(), &tex_desc);

    WGPUTextureViewDescriptor view_desc = {
        .label = "Scene UI Depth View",
        .format = TEXTURE_FORMAT_DEPTH,
        .dimension = WGPUTextureViewDimension_2D,
        .aspect = WGPUTextureAspect_DepthOnly,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
    };

    ui->depth_view = wgpuTextureCreateView(ui->depth_texture, &view_desc);
  }

  // Icon Atlas
  {
    TextureAtlasDescriptor atlas_desc = {
        .cell_count = {16, 16},
        .cell_size = {128, 128},
        .format = TEXTURE_FORMAT_OFFSCREEN,
        .label = "Scene UI Icon Atlas",
        .path = "./resources/assets/texture/ui/icon_atlas.png",
    };
    texture_atlas_create(&ui->atlas_texture, &atlas_desc);
  }
}

void scene_editor_ui_create_right_panel(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(
      ImVec2(ui->size[SceneEditorUISize_Screen_Width] -
                 ui->size[SceneEditorUISize_RightPanel_Width],
             0));

  ImGui::SetNextWindowSize(ImVec2(ui->size[SceneEditorUISize_RightPanel_Width],
                                  ui->size[SceneEditorUISize_Screen_Height]));

  ImGui::Begin("Left Panel", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
  {
    UI::Tree(scene, "Hierarchy").draw();
    UI::Inspector(scene, "Inspector").draw();
  }

  ImGui::End();
}

void scene_editor_ui_create_bottom_panel(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(
      ImVec2(0, ui->size[SceneEditorUISize_Screen_Height] -
                    ui->size[SceneEditorUISize_BottomPanel_Height]));

  ImGui::SetNextWindowSize(
      ImVec2(ui->size[SceneEditorUISize_Screen_Width] -
                 ui->size[SceneEditorUISize_RightPanel_Width],
             ui->size[SceneEditorUISize_BottomPanel_Height]));

  ImGui::Begin("Bottom Panel", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
  {

    UI::Log(scene, "Log").draw();
    ImGui::SameLine();
    UI::Browser(scene, "Browser").draw();
  }

  ImGui::End();
}

void scene_editor_ui_create_top_bar(SceneEditorUI *ui, Scene *scene) {

  const int top_bar_width = ui->size[SceneEditorUISize_Screen_Width] -
                            ui->size[SceneEditorUISize_RightPanel_Width];

  UI::RenderMode render_mode_buttons = UI::RenderMode(scene, "Render mode");

  ImGui::SetNextWindowPos(ImVec2(
      top_bar_width - (render_mode_buttons.count + 0.8) *
                          ui->size[SceneEditorUISize_Button_RenderModeSize],
      0));

  render_mode_buttons.draw();
}
