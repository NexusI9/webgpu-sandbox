#include "camera.h"

#include <stddef.h>
#include <stdint.h>

#include "resources/loader/loader.mbin.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/group.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/topology/base.h"
#include "runtime/mesh/topology/wireframe.h"
#include "runtime/mesh/transform.h"
#include "runtime/primitive/core.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/object/builder/billboard.h"
#include "runtime/scene/editor/object/builder/wireframe.h"
#include "runtime/scene/editor/object/list/list.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "utils/color.h"
#include "runtime/camera/core.h"

void seo_camera_create(SceneEditorObject *seo, Camera *camera,
                       const SEOCreateDescriptor *desc) {

  // define target
  seo->scene = desc->scene;

  const uint8_t seo_mesh_count = 2;
  seo_mesh_list_create(&seo->meshes, seo_mesh_count);

  // create new mesh in the mesh list
  SceneEditorObjectMesh *icon = seo_mesh_list_new_entry(&seo->meshes);
  icon->mesh = scene_new_mesh(desc->scene);
  icon->target = camera;
  icon->target_list_index = desc->target_list_index;

  const char *texture_path = "./resources/assets/texture/ui/camera.png";

  // create icon mesh
  seo_create_billboard(icon->mesh, &(SEOCreateBillboardDescriptor){
                                       .texture_path = texture_path,
                                       .position = &camera->position,
                                       .scale = &SEO_BILLBOARD_SCALE,
                                   });

  // create box mesh
  Primitive cube_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &cube_primitive,
  });

  // set callback
  icon->transform_callback[GizmoMode_Position] =
      seo_camera_set_position;
  icon->transform_callback[GizmoMode_Rotation] =
      seo_camera_set_rotation;
  icon->transform_callback[GizmoMode_Scale] = seo_camera_set_scale;

  SceneEditorObjectMesh *cube = seo_mesh_list_new_entry(&seo->meshes);
  cube->mesh = scene_new_mesh(desc->scene);

  // create manually wirerfame since seo is part of fixed rendering, so the
  // mesh topology generation isn't automatically handled.
  seo_create_wireframe(cube->mesh,
                       &(SEOCreateWireframeDescriptor){
                           .color = &(color){1.0f, 0.7f, 0.4f, 1.0f},
                           .thickness = SEO_WIREFRAME_LINE_THICKNESS,
                           .vertex = &cube_primitive.vertex,
                           .index = &cube_primitive.index,
                           .name = "seo camera",
                       });

  // init vertex groups
  VertexGroupSet *cube_group = &cube->mesh->topology.base.group;
  if (vertex_group_set_create(cube_group, VERTEX_GROUP_CAPACITY_DEFAULT) ==
      VertexGroupStatus_Success) {

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
  mesh_set_position(cube->mesh, (vec3){0.0f, 1.0f, 0.0f});

  // set callback
  cube->transform_callback[GizmoMode_Position] =
      seo_camera_set_position;
  cube->transform_callback[GizmoMode_Rotation] =
      seo_camera_set_rotation;
  cube->transform_callback[GizmoMode_Scale] = seo_camera_set_scale;

  seo->origin = icon->mesh;

  // set fov deformation
  seo_camera_fov(seo, 90.0f);
}

void seo_camera_set_position(SEOTransformCallback *desc) {

  // transform target
  camera_set_position(desc->mesh->target, desc->offset);

  // transform mesh
  for (size_t i = 0; i < desc->seo->meshes.length; i++)
    mesh_set_position(desc->seo->meshes.entries[i].mesh, desc->offset);
}

void seo_camera_set_rotation(SEOTransformCallback *desc) {}

void seo_camera_set_scale(SEOTransformCallback *desc) {}

void seo_camera_lookat(SceneEditorObject *seo, vec3 position, vec3 target) {

  // update icon position
  Mesh *icon = seo->meshes.entries[0].mesh;
  mesh_set_position(icon, position);

  // update camera matrix
  // camera_lookat(seo->target, position, target);

  // update seo cube mesh rotation
  Mesh *cube = seo->meshes.entries[1].mesh;
  mesh_lookat(cube, position, target);
}

/**
   Deform camera seo mesh according to fov
   (Goes from cube to prism)
 */
void seo_camera_fov(SceneEditorObject *seo, float fov) {

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

  Mesh *cube = seo->meshes.entries[cube_mesh_id].mesh;
  // get vertex attributes + index for line mesh composition
  VertexAttribute *cube_base_attribute = mesh_topology_base(cube).attribute;

  VertexGroupSet *cube_group = &cube->topology.base.group;
  VertexGroup *back_face = vertex_group_set_find(cube_group, "back");

  mesh_topology_base_set_position(&cube->topology.base, back_face,
                                  &(vec3){0.0f, 0.0f, -1.0f});

  mesh_topology_base_set_scale(&cube->topology.base, back_face,
                               &(vec3){0.5f, 0.5f, 0.5f});

  // update wireframe topology according to base
  mesh_topology_wireframe_update(&cube->topology.base,
                                 &cube->topology.wireframe);
}
