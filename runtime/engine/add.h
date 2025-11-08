#ifndef _ENGINE_ADD_H_
#define _ENGINE_ADD_H_

#include "core.h"
#include "runtime/light/list.h"

typedef enum {
  EngineAddFlag_None = 0,
  EngineAddFlag_Hide = 1 << 0,
  EngineAddFlag_Unselectable = 1 << 1,
  EngineAddFlag_TreeHide = 1 << 2,
} EngineAddFlag;

Scene *engine_add_scene(Engine *, const SceneCreateDescriptor *);

// === Add Light ===
SceneEditorMeshList *engine_scene_add_point_light(Engine *,
                                                  PointLightDescriptor *,
                                                  const LightCreateFlag,
                                                  PointLight **);

SceneEditorMeshList *engine_scene_add_spot_light(Engine *,
                                                 SpotLightDescriptor *,
                                                 const LightCreateFlag,
                                                 SpotLight **);

SceneEditorMeshList *engine_scene_add_sun_light(Engine *, SunLightDescriptor *,
                                                const LightCreateFlag,
                                                SunLight **);

SceneEditorMeshList *engine_scene_add_ambient_light(Engine *,
                                                    AmbientLightDescriptor *,
                                                    AmbientLight **);

// === Add Probe ===
SceneEditorMeshList *engine_scene_add_probe_reflection_grid(
    Engine *, ProbeReflectionGridDescriptor *, ProbeReflectionGrid **);

SceneEditorMeshList *engine_scene_add_probe_reflection_plane(
    Engine *, ProbeReflectionPlaneDescriptor *, ProbeReflectionPlane **);

// === Add Camera ===
SceneEditorMeshList *
engine_scene_add_camera(Engine *, const CameraCreateDescriptor *, Camera **);

// === Add Mesh ===
EngineStatus engine_scene_add_mesh(Engine *, Mesh *, const char *,
                                   const EngineAddFlag);

EngineStatus engine_scene_add_mesh_ref_list(Engine *, MeshRefList *,
                                            const char *, const EngineAddFlag);

EngineStatus engine_scene_add_mesh_pipeline(Engine *, Mesh *,
                                            const RendererPipeline,
                                            const char *, const EngineAddFlag);

EngineStatus engine_scene_add_mesh_pipeline_ref_list(Engine *, MeshRefList *,
                                                     const RendererPipeline,
                                                     const char *,
                                                     const EngineAddFlag);

// === Remove Mesh ===
EngineStatus engine_scene_remove_mesh(Engine *, Mesh *);
EngineStatus engine_scene_remove_mesh_ref_list(Engine *, MeshRefList *);

#endif
