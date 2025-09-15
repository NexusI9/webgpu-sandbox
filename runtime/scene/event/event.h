#ifndef _SCENE_EVENT_H_
#define _SCENE_EVENT_H_

/**
   Scene Events are used to orchestrate the scene's main actors (camera, light,
   mesh) updates based on "triggers" (html inputs, gizmo transformation,
   animation).

   Previously the engine used a polling based system where each mesh
   uniform/texture and samplers where checked during the draw call to verify if
   they had a callback and if they required to be updated. The "CPU uniforms
   data" and "GPU uniform data" were compared during the loop all the time to
   check if they needed update

   In a effort of optimisation the engine now can update and rewrite
   bindgroups on the fly, meaning we do not depend on the main loop to update
   the uniforms and the update can be done anytime.

   However this new approach disable the "auto-check" system set priorly, which
   mean that we manualy need to write and decide when to update which uniforms.

   Those scene events aims to setup listeners callback in order to orchestrate
   which uniforms need to be updated based on which event.

   .- Example -----------------------------------------------------------------.
   |  On mouse move, the camera move, so we need to also  set a callback to    |
   |  update all meshes MVP uniforms.                                          |
   '---------------------------------------------------------------------------'

   However the previous is still available a I already foresee limitation with
   this one.

                                    *             *
                                       *      *
                                          *

                     Below is a scheme of the 2 available methods:

                            Method 1 (local): auto-update

                                .----------------.
                                |  Create Mesh   |
                                '-------.--------'
                                .-------'--------.
                                |  Create Shader |
                                '-------.--------'
                           .------------'------------.
                           |   Add Trigger/Callback  |
                           |      to uniform ƛ       |
                           '------------.------------'
                                .-------'--------.
                                |   Draw Mesh    |
                                '-------.--------'
                                       ...
                           .------------'-------------.
                           |         Trigger          | <----.
                           | (old_data == new_data ?) |      |
                           '------------.-------------'      |
                                        |                    |
                                     [ false ]                |
                                        |                    ∞
                           .------------'-------------.      |
                           |         Callback         |      |
                           |   (old_data = new_data ) |      |
                           '------------.-------------'      |
                           .------------'-------------.      |
                           |    Update uniform ƛ      |------'
                           '--------------------------'

       Fig1: During the draw call if the uniform has a trigger and callback,
       the trigger first check if the current data (reflecting the gpu value)
       is equal to the new one. If not, it calls the callback to update the
       old value to the new one and update the uniforms accordingly.

      ========================================================================

                        Method 2 (global): observer-update

                                  .-----------.
                                  |   Scene   |
                                  '-----.-----'
                .-----------------------+-----------------------.
                |                       |                       |
       .------- v -------.    .-------- v ------.    .--------- v --------.
       |   HTML events   |    | Gizmo transform |    |  Selection update  |
       '--------.--------'    '---------.-------'    '----------.---------'
                |                       |                       |
     .--------- v ---------. .--------- v ---------. .--------- v ---------.
     | + Update Uniform A  | | + Update Uniform C  | | + Update Uniform F  |
     | + Update Uniform B  | | + Update Uniform D  | | + Update Uniform G  |
     | + Update Uniform..  | |                     | | + Update Uniform H  |
     '---------------------' '---------------------' '---------------------'

    Fig2: The scene handles and orchestrates on a higher level its objects
    uniforms update depending on events set by the scene itself (html events,
    gizmo transformation)

 */

#include "core.h"
#include "event.html.h"

#endif
