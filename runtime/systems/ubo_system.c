#include "ubo_system.h"
#include "backend/ubo.h"

void ubo_system_register_draw_callback(UBOManager *ubo, Renderer *renderer) {

  renderer_add_draw_callback(renderer, ubo_system_draw_callback, (void *)ubo,
                             RendererDrawMode_Texture | RendererDrawMode_Solid |
                                 RendererDrawMode_Wireframe |
                                 RendererDrawMode_Boundbox);
}

void ubo_system_draw_callback(Renderer *renderer, void *data) {

  UBOManager *manager = (UBOManager *)data;

  for (UBOType type = 0; type < UBO_TYPE_COUNT; type++) {
    UBOBufferUpdateQueue *queue = &manager->buffers[type].update_queue;

    while (queue->length > 0) {
      ubo_id_t id = queue->entries[0];

      ubo_upload_entry(manager, type,
                       &(UBOSlot){
                           .id = id,
                           .uniform = &manager->buffers[type].entries[id],
                       });
      ubo_update_queue_shift(manager, type);
    }
  }
}
