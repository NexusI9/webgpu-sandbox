#include "camera.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/geometry/line/line.h"
#include "../runtime/geometry/vertex/vertex.h"
#include "../utils/system.h"
#include "../runtime/scene/scene.h"

#include <stddef.h>

void seo_camera_create(SceneEditorObject *seo, Camera *camera,
                       const SEOCreateDescriptor *desc) {

  // define target
  seo->target = camera;

  const size_t seo_mesh_count = 2;
  mesh_ref_list_create(&seo->meshes, seo_mesh_count);

  // create new mesh in the mesh list
  Mesh *icon = scene_new_mesh(desc->scene);
  const char *texture_path = "./resources/assets/texture/ui/camera.png";

  // create icon mesh
  seo_create_billboard(icon, &(SEOCreateBillboardDescriptor){
                                 .texture_path = texture_path,
                                 .device = desc->device,
                                 .queue = desc->queue,
                                 .position = &camera->position,
                                 .scale = &SEO_BILLBOARD_SCALE,
                             });

  // store mesh pointer in seo mesh ref list
  mesh_ref_list_insert(&seo->meshes, icon);

  // create box mesh
  Primitive cube_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &cube_primitive,
  });

  Mesh *cube = scene_new_mesh(desc->scene);

  // create manually wirerfame since seo is part of fixed rendering, so the
  // mesh topology generation isn't automatically handled.
  seo_create_wireframe(cube, &(SEOCreateWireframeDescriptor){
                                 .device = desc->device,
                                 .queue = desc->queue,
                                 .color = &(vec3){1.0f, 0.7f, 0.4f},
                                 .thickness = SEO_WIREFRAME_LINE_THICKNESS,
                                 .vertex = &cube_primitive.vertex,
                                 .index = &cube_primitive.index,
                                 .name = "seo camera",
                             });

  // init vertex groups
  VertexGroupSet *cube_group = &cube->topology.base.group;
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
  mesh_translate(cube, (vec3){0.0f, 1.0f, 0.0f});

  mesh_ref_list_insert(&seo->meshes, cube);

  // set fov deformation
  seo_camera_fov(seo, 90.0f);

  // set callback
  seo->transform_callback[GizmoTransformMode_Translate] = seo_camera_translate;
  seo->transform_callback[GizmoTransformMode_Rotate] = seo_camera_rotate;
  seo->transform_callback[GizmoTransformMode_Scale] = seo_camera_scale;

  seo->scene = desc->scene;
}

void seo_camera_translate(SceneEditorObject *seo, vec3 value) {

  // transform target
  camera_translate(seo->target, value);

  // transform mesh
  mesh_ref_list_translate(&seo->meshes, value);
}

void seo_camera_rotate(SceneEditorObject *seo, vec3 value) {}

void seo_camera_scale(SceneEditorObject *seo, vec3 value) {}

void seo_camera_lookat(SceneEditorObject *seo, vec3 position, vec3 target) {

  // update icon position
  Mesh *icon = seo->meshes.entries[0];
  mesh_translate(icon, position);

  // update camera matrix
  camera_lookat(seo->target, position, target);

  // update seo cube mesh rotation
  Mesh *cube = seo->meshes.entries[1];
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

  Mesh *cube = seo->meshes.entries[cube_mesh_id];
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
