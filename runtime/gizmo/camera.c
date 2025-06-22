#include "camera.h"
#include "../geometry/vertex/vertex.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/geometry/line/line.h"
#include "../utils/system.h"
#include "billboard.h"
#include "wireframe.h"
#include <stddef.h>

void gizmo_camera_create(GizmoCamera *gizmo, Camera *camera,
                         const GizmoCreateDescriptor *desc) {

  // define target
  gizmo->target = camera;

  const size_t gizmo_mesh_count = 2;
  mesh_reference_list_create(&gizmo->meshes, gizmo_mesh_count);

  // create new mesh in the mesh list
  Mesh *icon = mesh_list_new_mesh(desc->list);
  const char *texture_path = "./resources/assets/texture/ui/camera.png";

  // create icon mesh
  gizmo_create_billboard(icon, &(GizmoCreateBillboardDescriptor){
                                   .texture_path = texture_path,
                                   .device = desc->device,
                                   .queue = desc->queue,
                                   .position = &camera->position,
                                   .scale = &GIZMO_BILLBOARD_SCALE,
                               });

  // store mesh pointer in gizmo mesh ref list
  mesh_reference_list_insert(&gizmo->meshes, icon);

  // create box mesh
  Primitive cube_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &cube_primitive,
  });

  Mesh *cube = mesh_list_new_mesh(desc->list);

  // create manually wirerfame since gizmo is part of fixed rendering, so the
  // mesh topology generation isn't automatically handled.
  gizmo_create_wireframe(cube, &(GizmoCreateWireframeDescriptor){
                                   .device = desc->device,
                                   .queue = desc->queue,
                                   .color = &(vec3){1.0f, 0.7f, 0.4f},
                                   .thickness = GIZMO_WIREFRAME_LINE_THICKNESS,
                                   .vertex = &cube_primitive.vertex,
                                   .index = &cube_primitive.index,
                                   .name = "gizmo camera",
                               });

  // init vertex groups
  VertexGroupSet *cube_group = &cube->topology.base.group;
  if (vertex_group_set_create(cube_group, VERTEX_GROUP_CAPACITY_DEFAULT) ==
      VERTEX_SUCCESS) {

    vertex_group_set_insert(cube_group,
                            &(VertexGroup){
                                .name = "front",
                                .entries = (vindex_t[]){0, 2, 4, 10},
                                .length = 4,
                                .capacity = 4,
                            });

    vertex_group_set_insert(cube_group, &(VertexGroup){
                                            .name = "back",
                                            .entries = (vindex_t[]){1, 9, 7, 8},
                                            .length = 4,
                                            .capacity = 4,
                                        });
  }

  // translate cube upward
  mesh_translate(cube, (vec3){0.0f, 1.0f, 0.0f});

  mesh_reference_list_insert(&gizmo->meshes, cube);

  // set fov deformation
  gizmo_camera_fov(gizmo, 90.0f);
}

void gizmo_camera_translate(GizmoCamera *gizmo, vec3 position) {

  // transform target
  camera_translate(gizmo->target, position);

  // transform mesh
  mesh_reference_list_translate(&gizmo->meshes, position);
}

void gizmo_camera_rotate(GizmoCamera *gizmo, vec3 rotation) {}

void gizmo_camera_scale(GizmoCamera *gizmo, vec3 scale) {}

void gizmo_camera_lookat(GizmoCamera *gizmo, vec3 position, vec3 target) {

  // update icon position
  Mesh *icon = gizmo->meshes.entries[0];
  mesh_translate(icon, position);

  // update camera matrix
  camera_lookat(gizmo->target, position, target);

  // update gizmo cube mesh rotation
  Mesh *cube = gizmo->meshes.entries[1];
  mesh_lookat(cube, position, target);
}

/**
   Deform camera gizmo mesh according to fov
   (Goes from cube to prism)
 */
void gizmo_camera_fov(GizmoCamera *gizmo, float fov) {

  /**
        checked on blender to get faces vertex indices:

             4.-----------.0     Back (CW):  4, 0, 1, 5
             /|          /|      Front (CW): 6, 2, 3, 7
            / |         / |
          6'--+--------'2 |
           |  |        |  |
           | 5'--------|--'1
           | /         | /
           |/          |/
          7'-----------'3

   */

  size_t cube_mesh_id = 1;

  Mesh *cube = gizmo->meshes.entries[cube_mesh_id];
  // get vertex attributes + index for line mesh composition
  VertexAttribute *cube_base_attribute = mesh_topology_base(cube).attribute;

  VertexGroupSet *cube_group = &cube->topology.base.group;
  VertexGroup *back_face = vertex_group_set_find(cube_group, "back");

  mesh_topology_base_translate(&cube->topology.base, back_face,
                               &(vec3){0.0f, 0.0f, -1.0f});

  mesh_topology_base_scale(&cube->topology.base, back_face,
                           &(vec3){0.5f, 0.5f, 0.5f});

  // update wireframe topology according to base
  mesh_topology_wireframe_update(&cube->topology.base,
                                 &cube->topology.wireframe, cube->queue);
}
