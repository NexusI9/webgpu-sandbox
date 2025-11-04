#ifndef _ENGINE_CORE_H_
#define _ENGINE_CORE_H_

#define ENGINE_NAME engine

/**
   The Engine implementation serves as a high level orchestration that
   automatically links up engine entities for systematized process (add, remove,
   move, update...).
   As a result instead of having:
   {
     scene_add_mesh();
     renderer_update_shadow_map();
     ubo_new_entry();
   }

   we would simply call engine_scene_add_mesh();
 */

#endif
