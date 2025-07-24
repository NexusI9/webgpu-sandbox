#ifndef _SCENE_LAYER_
#define _SCENE_LAYER_

#include "../mesh/mesh.h"

#define SCENE_LAYER_CAPACITY 16
#define SCENE_LAYER_SET_CAPACITY 128

#define SCENE_LAYER_SUCCESS 0
#define SCENE_LAYER_ALLOC_FAIL 1
#define SCENE_LAYER_UNDEF_ERROR 2
#define SCENE_LAYER_SET_UNFOUND 3

static const char *const SCENE_LAYER_DEFAULT = "Default";
static const char *const SCENE_LAYER_GIZMO_TRANSFORM = "Gizmo Transform";
static const char *const SCENE_LAYER_GIZMO_SELECTABLE = "Gizmo Selectable";
static const char *const SCENE_LAYER_GIZMO_UNSELECTABLE = "Gizmo Unselectable";

typedef struct {
  char *name;
  MeshRefList meshes;
} SceneLayer;

typedef struct {
  size_t length;
  size_t capacity;
  SceneLayer *entries;
} SceneLayerSet;

/*Layer*/
int scene_layer_create(SceneLayer *, const char *, size_t);
void scene_layer_empty(SceneLayer *);
void scene_layer_rename(SceneLayer *, const char *);
void scene_layer_free(SceneLayer *);
Mesh *scene_layer_insert(SceneLayer *, Mesh *);
Mesh *scene_layer_find(SceneLayer *, Mesh *);

/*Layer set*/
int scene_layer_set_create(SceneLayerSet *, size_t);
SceneLayer *scene_layer_set_create_layer(SceneLayerSet *, const char *);
SceneLayer *scene_layer_set_find(SceneLayerSet *, const char *);
int scene_layer_set_delete(SceneLayerSet *, const char *);
void scene_layer_set_free(SceneLayerSet *);
Mesh *scene_layer_set_insert_mesh(SceneLayerSet *, const char *, Mesh *);
void scene_layer_set_insert_mesh_ref_list(SceneLayerSet *, const char *, MeshRefList *);

/*Utils*/
void scene_layer_set_print_layer(SceneLayerSet *, const char*);
#endif
