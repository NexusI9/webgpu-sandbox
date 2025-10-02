#include "core.h"
#include "./imgui_style/style.carbon.hpp"
#include "backend/context.h"
#include "backend/logger.h"
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_wgpu.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/input/core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/selection/filter.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/editor/selection/utils.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/scene/show.h"
#include "runtime/texture/atlas.h"
#include "runtime/texture/core.h"
#include "stdio.h"
#include "utils.hpp"
#include <stdint.h>

/* TODO: make context available in the scene editor ui, but may interfere witht
 * the "pure C" approach since SceneEditorUI is included in Scene.
 */
static ImGuiContext *imgui_context;

typedef enum {
  SceneEditorUIDisplay_Layout = 1 << 0,
  SceneEditorUIDisplay_Activity = 1 << 1,
} SceneEditorUIDisplay;

/* ===  SIZES === */
#define SCENE_EDITOR_UI_SIZE_COUNT 16
typedef enum {
  SceneEditorUISize_Screen_Width,
  SceneEditorUISize_Screen_Height,
  SceneEditorUISize_RightPanel_Width,
  SceneEditorUISize_Tree_Height,
  SceneEditorUISize_TopBar_Height,
  SceneEditorUISize_TopBar_Margin,
  SceneEditorUISize_Gizmo_Width,
  SceneEditorUISize_Gizmo_Height,
  SceneEditorUISize_Gizmo_Margin,
  SceneEditorUISize_Button_RenderModeSize,
  SceneEditorUISize_Button_GizmoSize,
  SceneEditorUISize_Button_DisplaySize,
  SceneEditorUISize_BottomPanel_Height,
  SceneEditorUISize_Log_IconScale,
  SceneEditorUISize_Monitor_Width,
  SceneEditorUISize_Monitor_Height,
} SceneEditorUISize;

static int ui_size[SCENE_EDITOR_UI_SIZE_COUNT] = {
    [SceneEditorUISize_Screen_Width] = 0,
    [SceneEditorUISize_Screen_Height] = 0,
    [SceneEditorUISize_RightPanel_Width] = 250,
    [SceneEditorUISize_Tree_Height] = 400,
    [SceneEditorUISize_TopBar_Height] = 40,
    [SceneEditorUISize_TopBar_Margin] = 0,
    [SceneEditorUISize_Gizmo_Width] = 100,
    [SceneEditorUISize_Gizmo_Height] = 400,
    [SceneEditorUISize_Gizmo_Margin] = 10,
    [SceneEditorUISize_Button_RenderModeSize] = 15,
    [SceneEditorUISize_Button_GizmoSize] = 30,
    [SceneEditorUISize_Button_DisplaySize] = 24,
    [SceneEditorUISize_BottomPanel_Height] = 200,
    [SceneEditorUISize_Log_IconScale] = 16,
    [SceneEditorUISize_Monitor_Width] = 300,
    [SceneEditorUISize_Monitor_Height] = 100,
};

static inline void scene_editor_ui_set_icon_cell(SceneEditorUI *);
static inline void scene_editor_ui_scale_size(SceneEditorUI *);
static inline void scene_editor_ui_create_texture(SceneEditorUI *);

static inline bool scene_editor_ui_create_button_icon(SceneEditorUI *,
                                                      const SceneEditorUIIcon,
                                                      const char *, ImVec2);

static inline void scene_editor_ui_create_display(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_scene_tree(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_properties(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_gizmo(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_top_bar(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_left_panel(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_right_panel(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_bottom_panel(SceneEditorUI *,
                                                       Scene *);
static inline void scene_editor_ui_create_log(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_inspector(SceneEditorUI *, Scene *);
static inline void scene_editor_ui_create_monitor(SceneEditorUI *, Scene *);

SceneEditorUIStatus scene_editor_ui_init(SceneEditorUI *ui,
                                         const SceneEditorUIDescriptor *desc) {

  logger_add(LoggerFlag_Process, "Intitializing Editor UI");

  {
    ui->clock = desc->clock;
    ui->dpi = desc->dpi;
    scene_editor_ui_create_texture(ui);
    scene_editor_ui_scale_size(ui);
    scene_editor_ui_set_icon_cell(ui);
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

int display = SceneEditorUIDisplay_Activity | SceneEditorUIDisplay_Layout;
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
    io.MouseWheel = g_input.mouse.wheel.deltaX;
  }

  {
    ImGui_ImplWGPU_NewFrame();
    ImGui::NewFrame();
    {

      if (display & SceneEditorUIDisplay_Layout) {
        scene_editor_ui_create_top_bar(ui, scene);
        scene_editor_ui_create_right_panel(ui, scene);
        scene_editor_ui_create_bottom_panel(ui, scene);
        scene_editor_ui_create_gizmo(ui, scene);
      }

      if (display & SceneEditorUIDisplay_Activity) {
        scene_editor_ui_create_monitor(ui, scene);
      }

      scene_editor_ui_create_display(ui, scene);
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

void scene_editor_ui_scale_size(SceneEditorUI *ui) {

  ui_size[SceneEditorUISize_Screen_Width] = context_width();
  ui_size[SceneEditorUISize_Screen_Height] = context_height();

  for (uint16_t i = 0; i < SCENE_EDITOR_UI_SIZE_COUNT; i++)
    ui_size[i] *= ui->dpi;
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

bool scene_editor_ui_create_button_icon(SceneEditorUI *ui,
                                        const SceneEditorUIIcon icon,
                                        const char *id, ImVec2 scale) {

  SceneEditorUIIconUV *uv = &ui->icon_uv[icon];
  return ImGui::ImageButton(id, (ImTextureRef)ui->atlas_texture.view, scale,
                            ImVec2(uv->uv0[0], uv->uv0[1]),
                            ImVec2(uv->uv1[0], uv->uv1[1]));
}

static const ScenePipeline tree_meshes[3] = {
    ScenePipeline_Dynamic_Lit,
    ScenePipeline_Dynamic_LitShadow,
    ScenePipeline_Dynamic_Unlit,
};

void scene_editor_ui_create_right_panel(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(
      ImVec2(ui_size[SceneEditorUISize_Screen_Width] -
                 ui_size[SceneEditorUISize_RightPanel_Width],
             0));

  ImGui::SetNextWindowSize(ImVec2(ui_size[SceneEditorUISize_RightPanel_Width],
                                  ui_size[SceneEditorUISize_Screen_Height]));

  ImGui::Begin("Left Panel", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
  {
    scene_editor_ui_create_scene_tree(ui, scene);
    scene_editor_ui_create_properties(ui, scene);
  }

  ImGui::End();
}

void scene_editor_ui_create_display(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(ImVec2(ui_size[SceneEditorUISize_Gizmo_Margin], 0),
                          ImGuiCond_Always);
  ImGui::Begin("Display Frame", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);

  // remove backgrounds & padding
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
  {
    if (scene_editor_ui_create_button_icon(
            ui, SceneEditorUIIcon_Layout, "Layout",
            ImVec2(ui_size[SceneEditorUISize_Button_DisplaySize],
                   ui_size[SceneEditorUISize_Button_DisplaySize])))
      display ^= SceneEditorUIDisplay_Layout;

    ImGui::SameLine();

    if (scene_editor_ui_create_button_icon(
            ui, SceneEditorUIIcon_Activity, "Activity",
            ImVec2(ui_size[SceneEditorUISize_Button_DisplaySize],
                   ui_size[SceneEditorUISize_Button_DisplaySize])))
      display ^= SceneEditorUIDisplay_Activity;
  }
  ImGui::PopStyleColor(3);
  ImGui::PopStyleVar();
  ImGui::End();
}

void scene_editor_ui_create_bottom_panel(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(
      ImVec2(0, ui_size[SceneEditorUISize_Screen_Height] -
                    ui_size[SceneEditorUISize_BottomPanel_Height]));

  ImGui::SetNextWindowSize(
      ImVec2(ui_size[SceneEditorUISize_Screen_Width] -
                 ui_size[SceneEditorUISize_RightPanel_Width],
             ui_size[SceneEditorUISize_BottomPanel_Height]));

  ImGui::Begin("Bottom Panel", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
  {
    scene_editor_ui_create_log(ui, scene);
    ImGui::SameLine();
    scene_editor_ui_create_inspector(ui, scene);
  }

  ImGui::End();
}

typedef struct {
  const ThemeDefaultColor text;
  const ThemeDefaultColor timestamp;
  const ThemeDefaultColor background;
  SceneEditorUIIcon icon;
} LoggerLook;

static const LoggerLook logger_looks[LOGGER_FLAG_COUNT] = {
    [LoggerFlag_Print] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_Info] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Log_Info,
        },
    [LoggerFlag_Error] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_DANGER_ON_DANGER,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_DANGER_SUBTLE,
            .background = THEME_DEFAULT_COLOR_BACKGROUND_DANGER_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Error,
        },
    [LoggerFlag_Warning] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_WARNING_ON_WARNING,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_WARNING_SUBTLE,
            .background = THEME_DEFAULT_COLOR_BACKGROUND_WARNING_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Warning,
        },
    [LoggerFlag_Debug] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_ShaderCreate] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_MeshBuild] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_MeshCreate] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_Import] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_INFORMATION_ON_INFORMATION,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_INFORMATION_SUBTLE,
            .background =
                THEME_DEFAULT_COLOR_BACKGROUND_INFORMATION_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Import,
        },
    [LoggerFlag_Success] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_SUCCESS_ON_SUCCESS,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUCCESS_SUBTLE,
            .background = THEME_DEFAULT_COLOR_BACKGROUND_SUCCESS_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Success,
        },
    [LoggerFlag_Process] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Log_Process,
        },
};

void scene_editor_ui_create_log(SceneEditorUI *ui, Scene *scene) {

  static ImVec2 log_icon_scale =
      ImVec2(ui_size[SceneEditorUISize_Log_IconScale],
             ui_size[SceneEditorUISize_Log_IconScale]);

  ImGui::BeginChild("Logs", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0),
                    true);
  {
    ImGui::Text("Logs");
    ImGui::BeginChild("Logs entries", ImVec2(0, 0), true);
    {
      for (size_t i = 0; i < g_logger.length; i++) {

        const LoggerLook *look = &logger_looks[g_logger.flags[i]];
        const SceneEditorUIIconUV *uv = &ui->icon_uv[look->icon];
        const color *background_color = &theme_default_color[look->background];
        const color *timestamp_color = &theme_default_color[look->timestamp];
        const color *text_color = &theme_default_color[look->text];
        const char *message = g_logger.messages[i];

        ImVec2 pos = ImGui::GetCursorScreenPos();

        float row_height = ImGui::GetTextLineHeightWithSpacing();
        float row_width = ImGui::GetContentRegionAvail().x;

        ImGui::GetWindowDrawList()->AddRectFilled(
            pos, ImVec2(pos.x + row_width, pos.y + row_height),
            ImGui::ColorConvertFloat4ToU32((ImVec4 &)*background_color));

        ImGui::Image((ImTextureRef)ui->atlas_texture.view, log_icon_scale,
                     ImVec2(uv->uv0[0], uv->uv0[1]),
                     ImVec2(uv->uv1[0], uv->uv1[1]));
        ImGui::SameLine();
        ImGui::TextColored((ImVec4 &)*text_color, "%s", g_logger.messages[i]);
      }

      if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
  }
  ImGui::EndChild();
}

void scene_editor_ui_create_inspector(SceneEditorUI *ui, Scene *scene) {
  ImGui::BeginChild("Inspector", ImVec2(0, 0), true);
  {
    ImGui::Text("Inspector");
  }
  ImGui::EndChild();
}

void scene_editor_ui_create_properties(SceneEditorUI *ui, Scene *scene) {
  ImGui::BeginChild("Properties", ImVec2(0, 0), true);
  {
    ImGui::Separator();
    ImGui::Text("Properties");
  }
  ImGui::EndChild();
}

void scene_editor_ui_create_scene_tree(SceneEditorUI *ui, Scene *scene) {

  ImGui::BeginChild("Tree", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.3f),
                    true);
  ImGui::Text("Scene Inspector");

  {
    // === Meshes ===
    for (int i = 0; i < 3; i++) {
      MeshRefList *meshes = scene_pipeline(scene, tree_meshes[i]);
      for (int j = 0; j < meshes->length; j++) {
        Mesh *mesh = meshes->entries[j];

        // ImGuiTreeNodeFlags_SpanAvailWidth
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_AllowItemOverlap |
                                   ImGuiTreeNodeFlags_FramePadding;

        if (mesh->children.length == 0)
          flags |= ImGuiTreeNodeFlags_Leaf;

        // Tree item
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.f, 8.f));
        if (ImGui::TreeNodeEx(mesh->name, flags)) {

          if (ImGui::IsItemClicked())
            scene_selection_toggle_mesh(scene, mesh);

          if (mesh->children.length == 0)
            ImGui::TreePop();
        }
        ImGui::PopStyleVar();

        // Visibility icon
        {
          const float line_height = ImGui::GetTextLineHeightWithSpacing();
          const float icon_size =
              ui_size[SceneEditorUISize_Button_RenderModeSize];

          ImGui::SameLine(ImGui::GetWindowContentRegionMax().x -
                          ui->dpi * icon_size);

          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

          char button_id[256];
          snprintf(button_id, 256, "mesh_visibility_%u", mesh->id);
          if (scene_editor_ui_create_button_icon(ui, SceneEditorUIIcon_Eye,
                                                 button_id,
                                                 ImVec2(icon_size, icon_size)))
            scene_visibility_toggle_mesh(scene, mesh);

          ImGui::PopStyleColor(1);
        }
      }
    }

    // === Lights ===

    // === Probes ===
  }
  ImGui::EndChild();
}

typedef enum {
  SceneEditorUIMonitorType_FPS,
  SceneEditorUIMonitorType_CPU,
  SceneEditorUIMonitorType_GPU,
} SceneEditorUIMonitorType;

static float values[3][90] = {};
static int values_offset[3] = {0};
void scene_editor_ui_create_monitor(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(
      ImVec2(ui_size[SceneEditorUISize_Gizmo_Margin],
             ui_size[SceneEditorUISize_TopBar_Height] + 10));
  ImGui::SetNextWindowSize(ImVec2(ui_size[SceneEditorUISize_Monitor_Width],
                                  ui_size[SceneEditorUISize_Monitor_Height]));

  ImGui::Begin("Monitor", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);
  // === FPS ===
  {
    float fps = ImGui::GetIO().Framerate;
    float *value = values[SceneEditorUIMonitorType_FPS];
    int *offset = &values_offset[SceneEditorUIMonitorType_FPS];

    value[*offset] = fps;
    *offset =
        (*offset + 1) % IM_ARRAYSIZE(values[SceneEditorUIMonitorType_FPS]);

    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.0f", fps);

    ImGui::PlotLines(buf, value,
                     IM_ARRAYSIZE(values[SceneEditorUIMonitorType_FPS]),
                     *offset, nullptr, 0.0f, 120.0f, ImVec2(0, 80));
  }


  ImGui::End();
}

static const struct {
  const GizmoMode mode;
  const char *label;
  const SceneEditorUIIcon icon;
} gizmo_button[] = {
    {GizmoMode_Position, "Position", SceneEditorUIIcon_Gizmo_Position},
    {GizmoMode_Rotation, "Rotate", SceneEditorUIIcon_Gizmo_Rotate},
    {GizmoMode_Scale, "Scale", SceneEditorUIIcon_Gizmo_Scale},
};

void scene_editor_ui_create_gizmo(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(
      ImVec2(ui_size[SceneEditorUISize_Gizmo_Margin],
             (int)(ui_size[SceneEditorUISize_Screen_Height] / 2) -
                 (int)(ui_size[SceneEditorUISize_Gizmo_Height] / 2)));

  ImGui::SetNextWindowSize(ImVec2(ui_size[SceneEditorUISize_Gizmo_Width],
                                  ui_size[SceneEditorUISize_Gizmo_Height]));
  ImGui::Begin("Gizmo", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoBackground);

  for (uint8_t i = 0; i < 3; i++) {
    if (scene_editor_ui_create_button_icon(
            ui, gizmo_button[i].icon, gizmo_button[i].label,
            ImVec2(ui_size[SceneEditorUISize_Button_GizmoSize],
                   ui_size[SceneEditorUISize_Button_GizmoSize]))) {
      scene_gizmo_hide(scene);
      scene->editor.gizmo.transform.mode = gizmo_button[i].mode;
      if (scene_selection_length(&scene->editor.selection)) {
        scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                                     &scene->editor.selection,
                                     &scene->renderer.ssbo);
        scene_gizmo_show(scene);
      }
    }

    ImGui::Spacing();
  }

  ImGui::End();
}

static const struct {
  const SceneRendererDrawMode mode;
  const char *label;
  const SceneEditorUIIcon icon;
} render_button[] = {
    {
        SceneRendererDrawMode_Boundbox,
        "Boundbox",
        SceneEditorUIIcon_RenderMode_Boundbox,
    },
    {
        SceneRendererDrawMode_Wireframe,
        "Wireframe",
        SceneEditorUIIcon_RenderMode_Wireframe,
    },
    {
        SceneRendererDrawMode_Solid,
        "Solid",
        SceneEditorUIIcon_RenderMode_Solid,
    },
    {
        SceneRendererDrawMode_Texture,
        "Texture",
        SceneEditorUIIcon_RenderMode_Texture,
    },
};

void scene_editor_ui_create_top_bar(SceneEditorUI *ui, Scene *scene) {

  const int top_bar_width = ui_size[SceneEditorUISize_Screen_Width] -
                            ui_size[SceneEditorUISize_RightPanel_Width];
  const int button_count = 8;
  const int padding = 1 * ui->dpi;

  ImGui::SetNextWindowPos(ImVec2(ui_size[SceneEditorUISize_TopBar_Margin],
                                 ui_size[SceneEditorUISize_TopBar_Margin]));

  ImGui::PushStyleColor(ImGuiCol_WindowBg,
                        (ImVec4 &)*theme_default_color
                            [THEME_DEFAULT_COLOR_BACKGROUND_BLANKET_MEDIUM]);
  ImGui::SetNextWindowSize(
      ImVec2(top_bar_width, ui_size[SceneEditorUISize_TopBar_Height]));
  ImGui::Begin("Top bar", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  ImGui::SetCursorPosX(
      top_bar_width -
      button_count *
          (ui_size[SceneEditorUISize_Button_RenderModeSize] + padding));

  for (uint8_t i = 0; i < 4; i++) {
    if (scene_editor_ui_create_button_icon(
            ui, render_button[i].icon, render_button[i].label,
            ImVec2(ui_size[SceneEditorUISize_Button_RenderModeSize],
                   ui_size[SceneEditorUISize_Button_RenderModeSize])))
      scene_set_draw_mode(scene, render_button[i].mode);

    ImGui::SameLine();
  }
  ImGui::End();

  ImGui::PopStyleColor();
}
