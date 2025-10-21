#ifndef _PROBE_DRAW_H_
#define _PROBE_DRAW_H_

#include "backend/compute/kawase.h"
#include "backend/logger.h"
#include "backend/profiler.h"
#include "runtime/scene/core.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/scene/renderer/render_pass/visibility.h"
#include "webgpu/webgpu.h"

static inline void probe_reflection_plane_list_draw_callback(void *data) {

  // temp
  ProbeReflectionListDebug *debug = NULL;

  Scene *scene = (Scene *)data;
  ProbeReflectionPlaneList *list = &scene->planes_reflection;

  WGPUTextureView cached_view_color = list->pass.color.attachment.view;
  WGPUTextureView cached_view_depth = list->pass.depth.attachment.view;

  // then update probe list texture cube array based on each probes views
  profiler_latency_start(&scene->renderer.profiler,
                         ProfilerLatencyType_ReflectionPass);
  render_pass_im_begin(&list->pass);
  {
    for (size_t i = 0; i < list->length; i++) {

      ProbeReflectionPlane *probe = &list->entries[i];

      // prevent self reflection by disabling probe meshes from the render pass
      render_pass_disable_mesh_ref_list(&list->pass, &probe->excluded_meshes);

      // define target layer
      WGPUTextureViewDescriptor target_color_desc = {
          .label = "Probe Reflection Plane Target Color View",
          .arrayLayerCount = 1,
          .baseArrayLayer = probe->texture_layer,
          .dimension = WGPUTextureViewDimension_2D,
          .baseMipLevel = 0,
          .mipLevelCount = 1,
      };
      WGPUTextureView target_color =
          wgpuTextureCreateView(list->pass.color.texture, &target_color_desc);

      WGPUTextureViewDescriptor target_depth_desc = {
          .label = "Probe Reflection Plane Target Depth View",
          .arrayLayerCount = 1,
          .baseArrayLayer = probe->texture_layer,
          .dimension = WGPUTextureViewDimension_2D,
          .baseMipLevel = 0,
          .mipLevelCount = 1,
      };
      WGPUTextureView target_depth =
          wgpuTextureCreateView(list->pass.depth.texture, &target_depth_desc);

      // update each mesh views/projections matrix
      ProbeReflectionListPreprocessorData preprocessor_data = {
          .camera_offset = probe->ssbo_slot[ProbeReflectionSSBOField_Camera].id,
      };
      render_pass_update_all_preprocessor_data(&list->pass, &preprocessor_data);

      // draw pass
      RenderPassDrawOptions target_views = {
          .color = target_color,
          .depth = target_depth,
      };
      render_pass_im_set_views(&list->pass, &target_views);
      render_pass_im_draw(&list->pass);

      if (debug && probe->texture_layer < debug->max_views)
        scene_debug_view_create(debug->scene_debug, target_color);
      else
        wgpuTextureViewRelease(target_color);

      wgpuTextureViewRelease(target_depth);

      // re-enable all meshes for next draw (dirty......)
      render_pass_enable_all_mesh(&list->pass);
    }
  }
  render_pass_im_end(&list->pass);
  profiler_latency_end(&scene->renderer.profiler,
                       ProfilerLatencyType_ReflectionPass);

  // Kawase pass
  profiler_latency_start(&scene->renderer.profiler,
                         ProfilerLatencyType_KawasePass);
  {
    RenderPassDrawOptions src_views = {
        .color = cached_view_color,
        .depth = cached_view_depth,
    };
    render_pass_im_set_views(&list->pass, &src_views);

    KawaseDescriptor blur_desc = {
        .texture = list->pass.color.texture,
        .layer_count = list->length,
        .pass_count = 3,
    };
    compute_pass_kawase(&scene->renderer.draw.compute_pass, &blur_desc);
  }
  profiler_latency_end(&scene->renderer.profiler,
                       ProfilerLatencyType_KawasePass);
}

#endif
