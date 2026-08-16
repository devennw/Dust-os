#include "cmp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, CmpInitCmPrivateAlloc)
#pragma alloc_text(PAGE, CmpDestroyCmPrivateAlloc)
#pragma alloc_text(PAGE, CmpAllocateKeyControlBlock)
#pragma alloc_text(PAGE, CmpFreeKeyControlBlock)
#pragma alloc_text(INIT, CmpInitCmPrivateDelayAlloc)
#pragma alloc_text(PAGE, CmpDestroyCmPrivateDelayAlloc)
#pragma alloc_text(PAGE, CmpAllocateDelayItem)
#pragma alloc_text(PAGE, CmpFreeDelayItem)
#pragma alloc_text(PAGE, CmpFreeAllocateItem)
#endif

typedef struct _CM_ALLOC_PAGE {
  ULONG freeCount;
  ULONG reserved;
  PVOID allocPage;
  ULONG allocateItem;
} CM_ALLOC_PAGE, *PCM_ALLOC_PAGE;

#define _CM_KCBS_ENTRY_SISE sizeof(CM_CONTROL_KEY_BLOCK)
#define _CM_KBCS_PERPAGE ((PAGE_SIZE - OFFSET_FIELD(CM_ALLOC_PAGE, allocPage)) \
        / _CM_KBCS_ENTRY_SIZE)

#define KBC_TO_ALLOC_PAGE_ADDRESS(kbc) (PVOID)(((ULONGPTR) \
        (kbc)) & ~(PAGE_SIZE - 1))
#define KBC_TO_ALLOC_PAGE(kbc) ((PCM_ALLOC_PAGE)KBC_TO_ALLOC_PAGE_ADDRESS(kbc))

LIST_ENTRY cmpFreeListKBCHead;
BOOLEAN cmpInitedAlloc = FALSE;

KGUARD_MUTEX cmpAllocBucketLock;
