#include "filter.h"

#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/core.h"
#include "target_list.h"
#include "utils/dyli.h"
#include <stdint.h>

/**
   Transfert all the filter meshes to Scene Selection Object List (i.e. Selected
   meshes). We combine each mesh to their respective targets and insert the
   combination in the Selection List as to be proceed for transformation.

   We combine them into a single object as during the transformation process
   (hot loop) we require to sequentially access mesh + target + initial
   attribute.

   So not only packing them is clearer on a human understanding standpoint, but
   is also more cache friendly regarding the access pattern occuring during the
   transform loopback.

                    COLD PATH                                    HOT PATH

   .-----------.               .------------------.
   |   Mesh *  | ------.       | Selection Object |
   '-----------'       |       |------------------|
                       +-----> | - Mesh*          | ------> [[ Transform Cbk ]]
   .-----------.       |       | - Target*        |
   |  Target * |-------'       | - initial attr   |
   '-----------'               '------------------'


   */
void scene_selection_filter_set_all_active(SceneSelectionFilter *filter) {

  SceneSelectionObjectList *selection = &filter->selection;

  // map targets to mesh
  for (size_t i = 0; i < filter->meshes.length; i++) {
    Mesh *mesh = filter->meshes.entries[i];
    scene_selection_filter_selection_add_mesh(filter, mesh, &i);
  }

}

/**
   Empty all the filter objects to meshes[SceneSelectionState_Selected].
   */
void scene_selection_filter_set_all_inactive(SceneSelectionFilter *filter) {

  scene_selection_filter_selection_empty(filter);

}

