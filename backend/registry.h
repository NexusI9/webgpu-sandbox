#ifndef _UTILS_ID_H_
#define _UTILS_ID_H_

#include "backend/logger.h"
#define REG_MAX_OBJECTS 2048
#define REG_OWNER_UNDEFINED -1

typedef int reg_id_t;
typedef enum {
  RegEntryType_Mesh,
  RegEntryType_Camera,
  RegEntryType_AmbientLight,
  RegEntryType_PointLight,
  RegEntryType_SpotLight,
  RegEntryType_SunLight,
  RegEntryType_Scene,
  RegEntryType_SceneUI,
  RegEntryType_SceneLayer,
  RegEntryType_ProbeReflectionPlane,
  RegEntryType_ProbeReflectionGrid,
  RegEntryType_SceneEditorMesh,
  RegEntryType_SceneEditorMeshList,
  RegEntryType_SceneEditorMeshList_PointLight,
  RegEntryType_SceneEditorMeshList_AmbientLight,
  RegEntryType_SceneEditorMeshList_SunLight,
  RegEntryType_SceneEditorMeshList_SpotLight,
  RegEntryType_SceneEditorMeshList_PointLightShadow,
  RegEntryType_SceneEditorMeshList_SunLightShadow,
  RegEntryType_SceneEditorMeshList_SpotLightShadow,
  RegEntryType_SceneEditorMeshList_Camera,
  RegEntryType_SceneEditorMeshList_ProbeReflectionPlane,
  RegEntryType_SceneEditorMeshList_ProbeReflectionGrid,
  RegEntryType_RenderPipeline,
  RegEntryType_ComputePipeline,
  RegEntryType_Texture,
} RegEntryType;

typedef struct {
  reg_id_t id;
  RegEntryType type;
  void *ptr;
} RegEntry;

#ifdef __cplusplus
extern "C" {
#endif

// TODO: maybe segment reg into per-type, so it's faster to access the elements
// of a certain type RegEntry g_reg[TYPE_COUNT][REG_MAX_OBJECTS]
extern RegEntry g_reg[REG_MAX_OBJECTS];
extern reg_id_t g_reg_id;

reg_id_t reg_register(void *, RegEntryType);

static inline const int reg_length() { return g_reg_id; }

static inline const RegEntry *reg_lookup(reg_id_t id) {
  if (id >= REG_MAX_OBJECTS) {
    logger_add(LoggerFlag_Error, "id out of registry bounds.");
    return NULL;
  }

  return &g_reg[id];
}

static inline size_t reg_type_count(const RegEntryType type) {
  size_t count = 0;
  for (size_t i = 0; i < g_reg_id; i++)
    if (g_reg[i].type == type)
      count++;

  return count;
}

#ifdef __cplusplus
}
#endif

#endif
