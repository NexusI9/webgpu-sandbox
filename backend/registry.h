#ifndef _UTILS_ID_H_
#define _UTILS_ID_H_

#define REG_MAX_OBJECTS 1024

typedef int id_t;
typedef enum {
  RegEntryType_Mesh,
  RegEntryType_Camera,
  RegEntryType_AmbientLight,
  RegEntryType_PointLight,
  RegEntryType_SpotLight,
  RegEntryType_SunLight,
  RegEntryType_Scene,
} RegEntryType;

typedef struct {
  id_t id;
  RegEntryType type;
  void *ptr;
} RegEntry;

static RegEntry g_reg[REG_MAX_OBJECTS] = {0};

id_t reg_register(void *, RegEntryType);

void *reg_lookup(id_t);

#endif
