#include "core.h"
#include "runtime/scene/core.h"
#include "runtime/texture/core.h"
#include "stdio.h"

#include "./style/style.neon.hpp"
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_wgpu.h"
#include "webgpu/webgpu.h"

/* TODO: make context available in the scene editor ui, but may interfere witht
 * the "pure C" approach since SceneEditorUI is included in Scene.
 */
ImGuiContext *g_imgui_context;

static inline void scene_editor_ui_create_texture(SceneEditorUI *);
static inline void scene_editor_ui_create_scene_tree(SceneEditorUI *, Scene *);

SceneEditorUIStatus scene_editor_ui_init(SceneEditorUI *ui,
                                         const SceneEditorUIDescriptor *desc) {

  VERBOSE_PROCESS("Intitializing Editor UI");

  {
    ui->width = desc->width;
    ui->height = desc->height;
    ui->swapchain = desc->swapchain;
    ui->device = desc->device;
    ui->clock = desc->clock;
    ui->queue = desc->queue;

    scene_editor_ui_create_texture(ui);
  }

  {
    g_imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(g_imgui_context);
    scene_editor_ui_style_neon();
    
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
  }

  {
    ImGui_ImplWGPU_NewFrame();
    ImGui::NewFrame();
    scene_editor_ui_create_scene_tree(ui, scene);
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

void scene_editor_ui_create_texture(SceneEditorUI *ui) {

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

static const int tree_width = 300;
static const int tree_height = 400;
void scene_editor_ui_create_scene_tree(SceneEditorUI *ui, Scene *scene) {

  ImGui::SetNextWindowPos(ImVec2(*ui->width - tree_width, 0));
  ImGui::SetNextWindowSize(ImVec2(tree_width, tree_height));
  ImGui::Begin("Scene", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove);
  {
    size_t i;
    // === Meshes ===
    for (i = 0; i < scene->meshes.length; i++) {
      Mesh *mesh = &scene->meshes.entries[i];
      ImGuiTreeNodeFlags flags =
          ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
      ImGui::TreeNodeEx(mesh->name, flags);
    }

    // === Lights ===

    // === Probes ===
  }
  ImGui::End();
}
