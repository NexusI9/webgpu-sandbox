#ifndef _NAME_H_
#define _NAME_H_

#define NAME_LEN 1024
typedef char name_t[NAME_LEN];

static inline void name_copy(const char *src, name_t dest) {
  snprintf(dest, NAME_LEN, "%s", src);
}

static inline void name_compose(name_t dest, const char* fm, ...){
  va_list args;
  va_start(args, fm);
  vsnprintf(dest, NAME_LEN, fm, args);
  va_end(args);
}

#endif
