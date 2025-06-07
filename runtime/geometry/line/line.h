#ifndef _LINE_H_
#define _LINE_H_

/**
   LINES

   The line library focuses on providing utilities to create lines.
   Lines are primarily used for wireframes meshes. They offer greater
   flexibility (thickness control, universal).

   However converting mesh to a wireframe-friendly mesh require to recreate a
   whole new vertex "armature" going from "triangles" to "lines composed of
   triangles":

   Default mesh:                  Line mesh

            A                         A
           /|                     .--..--.
          / |                    /  / |  |
         /  |                   /  /  |  |  .------- Each unique edge become
        /   |                  /  /   |  | °         a thin quad (i.e. a line)
       /    |     ====>       /  /    |  |
      /     |                /  /     |  |
     /      |               /  /      |  |
    |_______|              '--'_______'--'
    B       C              B |________| C


   The lines get extruded during the shader, the vertex structure is the
   following:

    .-------------.------------.------------.------------------.
    |     vec3    |    vec3    |    vec3    |       vec2       |
    |-------------'------------'------------'------------------|
    |  X   Y   Z    X   Y   Z    R   G   B       S        A    |
    | '----------' '----------' '----------' '------' '------' |
    |     base       opposite      color       side   endpoint |
    '----------------------------------------------------------'

    A line (being a thin quad undercovered) is composed of 4 vertices:
    Each one having a specicific endpoint and side.

         CPU                                  GPU

         A,B                          A <---       ---> B
          x                            x-------;------x
          ;                            |       ;      |
          ;       ========>            |       ;      |
          ;                            |       ;      |
          ;                            |       ;      |
          x                            x-------;------x
         C,D                          C <---      ---> D


    Such approach complexify the initial mesh structure by adding N*2
    more vertex per edges.
    In an effort to make the mesh vertex adjustments easier, LineMesh provide
    functions to simply target an "anchor" (i.e. an index) and by adjusting this
    anchor, it will automatically adjust all the mesh line related vertex as
    well.

                anchor 0
                 --.--      1--------------    By moving the anchor 0,
                   |       /                   we will actually target
                '._|_.'   /                    the mesh indexes 1, 3, 26
                         3------------         and 28.
                26 __ 28
                |     |
                |     |
                |     |

     Anchors are simply created based on vertices that share the same positions.

 */

#include "core.h"
#include "mesh.h"

#endif
