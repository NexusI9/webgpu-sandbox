#ifndef _UTILS_FLAG_H_
#define _UTILS_FLAG_H_

static inline unsigned int flag_enable(unsigned int flag, unsigned int *dest) {
  return *dest |= flag;
}

static inline unsigned int flag_disable(unsigned int flag, unsigned int *dest) {
  return *dest &= ~flag;
}

#endif
