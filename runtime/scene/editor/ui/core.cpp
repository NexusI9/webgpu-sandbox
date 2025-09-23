#include "core.h"
#include "./style/style.carbon.hpp"
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_wgpu.h"
#include "runtime/input/core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/editor/selection/utils.h"
#include "runtime/texture/core.h"
#include "stdio.h"

/* TODO: make context available in the scene editor ui, but may interfere witht
 * the "pure C" approach since SceneEditorUI is included in Scene.
 */
static ImGuiContext *imgui_context;

static inline void scene_editor_ui_set_icon_cell(SceneEditorUI *);
static inline void scene_editor_ui_create_texture(SceneEditorUI *);

static inline bool scene_editor_ui_create_button_icon(SceneEditorUI *,
                                                      const SceneEditorUIIcon,
                                                      const char *, ImVec2);
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

  VERBOSE_PROCESS("Intitializing Editor UI");

  {
    ui->width = desc->width;
    ui->height = desc->height;
    ui->dpi = desc->dpi;
    ui->swapchain = desc->swapchain;
    ui->device = desc->device;
    ui->clock = desc->clock;
    ui->queue = desc->queue;

    scene_editor_ui_create_texture(ui);
    scene_editor_ui_set_icon_cell(ui);
  }

  {
    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);
    scene_editor_ui_style_carbon();

    ImGui_ImplWGPU_InitInfo info;

    info.Device = ui->device;
    info.RenderTargetFormat = TEXTURE_FORMAT_ONSCREEN_DEFAULT;
    info.DepthStencilFormat = TEXTURE_FORMAT_DEPTH_DEFAULT;
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
      wgpuDeviceCreateCommandEncoder(ui->device, &com_enc_desc);

  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(*ui->swapchain);

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
    io.DisplaySize.x = (float)*ui->width;
    io.DisplaySize.y = (float)*ui->height;
    io.DeltaTime = ui->clock->delta;
    io.DisplayFramebufferScale.x = 1.0f;
    io.DisplayFramebufferScale.y = 1.0f;
    io.FontGlobalScale = 1.0f;
    io.MousePos = ImVec2(g_input.mouse.x, g_input.mouse.y);
    io.MouseDown[0] = g_input.mouse.state;
    io.MouseWheel = g_input.mouse.wheel.deltaX;
  }

  {
    ImGui_ImplWGPU_NewFrame();
    ImGui::NewFrame();
    {
      scene_editor_ui_create_top_bar(ui, scene);
      scene_editor_ui_create_right_panel(ui, scene);
      scene_editor_ui_create_bottom_panel(ui, scene);
      scene_editor_ui_create_gizmo(ui, scene);
      scene_editor_ui_create_monitor(ui, scene);
    }
    ImGui::Render();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), ui->pass_encoder);
  }

  {
    wgpuRenderPassEncoderEnd(ui->pass_encoder);
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(command_encoder, NULL);
    wgpuQueueSubmit(ui->queue, 1, &command_buffer);
    wgpuTextureViewRelease(swapchain_view);
  }
}

void scene_editor_ui_set_icon_cell(SceneEditorUI *ui) {

  // define icon position on atlas
  ui->icon_uv[SceneEditorUIIcon_RenderMode_Boundbox] = {.cell = {5, 0}};
  ui->icon_uv[SceneEditorUIIcon_RenderMode_Wireframe] = {.cell = {6, 0}};
  ui->icon_uv[SceneEditorUIIcon_RenderMode_Solid] = {.cell = {7, 0}};
  ui->icon_uv[SceneEditorUIIcon_RenderMode_Texture] = {.cell = {8, 0}};
  ui->icon_uv[SceneEditorUIIcon_Gizmo_Position] = {.cell = {9, 0}};
  ui->icon_uv[SceneEditorUIIcon_Gizmo_Rotate] = {.cell = {10, 0}};
  ui->icon_uv[SceneEditorUIIcon_Gizmo_Scale] = {.cell = {11, 0}};

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
                .width = (uint32_t)*ui->width,
                .height = (uint32_t)*ui->height,
                .depthOrArrayLayers = 1,
            },
        .mipLevelCount = 1,
        .sampleCount = 1,
        .format = TEXTURE_FORMAT_DEPTH_DEFAULT,
        .usage = WGPUTextureUsage_RenderAttachment,
    };

    ui->depth_texture = wgpuDeviceCreateTexture(ui->device, &tex_desc);

    WGPUTextureViewDescriptor view_desc = {
        .label = "Scene UI Depth View",
        .format = TEXTURE_FORMAT_DEPTH_DEFAULT,
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
        .format = TEXTURE_FORMAT_OFFSCREEN_DEFAULT,
        .device = ui->device,
        .queue = ui->queue,
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

/* ===  SIZES === */
static const int right_panel_width = 250;
static const int tree_height = 400;

static const int top_bar_height = 40;
static const int top_bar_margin = 0;

static const int gizmo_width = 100;
static const int gizmo_height = 400;
static const int gizmo_margin = 10;

static const int button_render_mode_size = 15;
static const int button_gizmo_size = 30;

static const int bottom_panel_height = 200;

static const ScenePipeline tree_meshes[3] = {
    ScenePipeline_Dynamic_Lit,
    ScenePipeline_Dynamic_LitShadow,
    ScenePipeline_Dynamic_Unlit,
};

void scene_editor_ui_create_right_panel(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(ImVec2(*ui->width - right_panel_width, 0));
  ImGui::SetNextWindowSize(ImVec2(right_panel_width, *ui->height));
  ImGui::Begin("Left Panel", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
  {
    scene_editor_ui_create_scene_tree(ui, scene);
    scene_editor_ui_create_properties(ui, scene);
  }

  ImGui::End();
}

void scene_editor_ui_create_bottom_panel(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(ImVec2(0, *ui->height - bottom_panel_height));
  ImGui::SetNextWindowSize(
      ImVec2(*ui->width - right_panel_width, bottom_panel_height));
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

void scene_editor_ui_create_log(SceneEditorUI *ui, Scene *scene) {
  ImGui::BeginChild("Logs", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0),
                    true);
  {
    ImGui::Text("Logs");
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
        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (mesh->children.length == 0)
          flags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::PushID(mesh->id);
        ImGui::TreeNodeEx(mesh->name, flags);
        ImGui::PopID();

        if (mesh->children.length == 0)
          ImGui::TreePop();
      }
    }

    // === Lights ===

    // === Probes ===
  }
  ImGui::EndChild();
}

static float values[90] = {};
static int values_offset = 0;
void scene_editor_ui_create_monitor(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(ImVec2(gizmo_margin, top_bar_height + 10));
  ImGui::SetNextWindowSize(ImVec2(300, 100));
  ImGui::Begin("Monitor", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);
  // === FPS ===
  {
    float fps = ImGui::GetIO().Framerate;
    values[values_offset] = fps;
    values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);

    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.0f", fps);

    ImGui::PlotLines(buf, values, IM_ARRAYSIZE(values), values_offset, nullptr,
                     0.0f, 120.0f, ImVec2(0, 80));
  }

  ImGui::End();
}

void scene_editor_ui_create_gizmo(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(
      ImVec2(gizmo_margin, (int)(*ui->height / 2) - (int)(gizmo_height / 2)));
  ImGui::SetNextWindowSize(ImVec2(gizmo_width, gizmo_height));
  ImGui::Begin("Gizmo", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoBackground);
  {
    if (scene_editor_ui_create_button_icon(
            ui, SceneEditorUIIcon_Gizmo_Position, "Position",
            ImVec2(button_gizmo_size, button_gizmo_size))) {
      scene_gizmo_hide(scene);
      scene->editor.gizmo.transform.mode = GizmoMode_Position;
      scene_gizmo_show(scene);
    }

    ImGui::Spacing();

    if (scene_editor_ui_create_button_icon(
            ui, SceneEditorUIIcon_Gizmo_Rotate, "Rotate",
            ImVec2(button_gizmo_size, button_gizmo_size))) {
      scene_gizmo_hide(scene);
      scene->editor.gizmo.transform.mode = GizmoMode_Rotation;
      scene_gizmo_show(scene);
    }

    ImGui::Spacing();

    if (scene_editor_ui_create_button_icon(
            ui, SceneEditorUIIcon_Gizmo_Scale, "Scale",
            ImVec2(button_gizmo_size, button_gizmo_size))) {
      scene_gizmo_hide(scene);
      scene->editor.gizmo.transform.mode = GizmoMode_Scale;
      scene_gizmo_show(scene);
    }
  }
  ImGui::End();
}

void scene_editor_ui_create_top_bar(SceneEditorUI *ui, Scene *scene) {
  ImGui::SetNextWindowPos(ImVec2(top_bar_margin, top_bar_margin));

  ImGui::SetNextWindowSize(ImVec2(
      *ui->width - right_panel_width - 2 * top_bar_margin, top_bar_height));
  ImGui::Begin("Top bar", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
  {
    scene_editor_ui_create_button_icon(
        ui, SceneEditorUIIcon_RenderMode_Boundbox, "Boundbox",
        ImVec2(button_render_mode_size, button_render_mode_size));

    ImGui::SameLine();

    scene_editor_ui_create_button_icon(
        ui, SceneEditorUIIcon_RenderMode_Wireframe, "Wireframe",
        ImVec2(button_render_mode_size, button_render_mode_size));

    ImGui::SameLine();

    scene_editor_ui_create_button_icon(
        ui, SceneEditorUIIcon_RenderMode_Solid, "Solid",
        ImVec2(button_render_mode_size, button_render_mode_size));

    ImGui::SameLine();

    scene_editor_ui_create_button_icon(
        ui, SceneEditorUIIcon_RenderMode_Texture, "Texture",
        ImVec2(button_render_mode_size, button_render_mode_size));
  }
  ImGui::End();
}
