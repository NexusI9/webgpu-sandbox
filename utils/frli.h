#ifndef _FREE_LIST_H_
#define _FREE_LIST_H_

typedef enum {
  FreeListStatus_Success,
  FreeListStatus_AllocFail,
  FreeListStatus_NotInit,
  FreeListStatus_OutOfBound,
  FreeListStatus_UnfoundEntry,
  FreeListStatus_UndefError
} FreeListStatus;

#endif
