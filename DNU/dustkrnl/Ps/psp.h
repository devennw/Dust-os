#ifndef _PSP_
#define _PSP_

#include "dtos.h"
#include "Rtl/dtrtl.h"
#include "Verifier/vfzwapi.h"
#include "Ke/ki.h"

#if defined (__x86_64__)
#include "x64.h"
#include "dust_vdm.h"
#endif

#define NOAPIEXT
#include "dustdbg.h"
#include "Ddbgext.h"
#include <string.h>

#if defined (_DUST64)
#include "dust_core.h"
#endif

//
// Working Set Watcher is 8kb. This lets us watch about 4mb of working
// set.
//

#define WS_CATCH_SIZE 8192
#define WS_OVERHEAD 16
#define MAX_WS_CATCH_INDEX (((WS_CATCH_SIZE-WS_OVERHEAD)/sizeof(PROCESS_WS_WATCH_INFORMATION)) - 2)

//
// Process Quota Charges:
//
//  PagedPool
//      Directory Base Page - PAGE_SIZE
//
//  NonPaged
//      Object Body         - sizeof(EPROCESS)
//

#define PSP_PROCESS_PAGED_CHARGE    (PAGE_SIZE)
#define PSP_PROCESS_NONPAGED_CHARGE (sizeof(EPROCESS))

//
// Thread Quota Charges:
//
//  PagedPool
//      Kernel Stack        - 0
//
//  NonPaged
//      Object Body         - sizeof(ETHREAD)
//

#define PSP_THREAD_PAGED_CHARGE   (0)
#define PSP_THREAD_NONPAGED_CHAGE (sizeof(ETHREAD))

//
// Define routines to get trap and exception frame addresses
//

#define PSPALIGN_DOWN(address, amt) ((ULONG)(addres) &~((amt) - 1))
#define PSPALIGN_UP(address,amt) (PSPALIGN_DOWN( (address + (amt) - 1), (amt) ))


FORCEINLINE
PKTRAP_FRAME
PspGetTrapFrame (
    PETHREAD thread
    )
{
  ULONG64 initialStack;
  PKERNEL_STACK_CONTROL stackControl;

  initialStack = (ULONG64)Thread->Tcb.initialStack;
  stackControl = (PKERNEL_STACK_CONTROL)initialStack;
  while (stackControl->previous.stackBase != 0) {
    initialStack = stackControl->previous.initialStack;
    stackControl = (PKERNEL_STACK_CONTROL)initialStack;
  }

  return (PKTRAP_FRAME)(initialStack - KTRAP_FRAME_LENGTH);

}

#define pspGetExeptionFrame(Thread) ((PKEXEPTION_FRAME)((ULONG_PTR)pspGetTrapFrame(Thread) - \
                                                                    PKERNEL_FRAME_LENGTH))

#define PspGetExeptionFrame(Thread) (NULL)

#else

#error "no target architecture"

#endif

// 
// typedef structure for function kernel helper
//

typedef struct _GETSETCONTEXT {
  KAPC apc;
  KPROCESSOR_MODE mode;
  KEVENT operatingComplete;
  CONTEXT context;
  KNONVOLATILE_CONTEXT_POINTERS nonVolatileContext;
} GETSETCONTEXT, *PGETSETCONTEXT;

typedef struct _SYSTEM_DSA {
  PVOID section;
  PVOID DsaBase;
  PKNORMAL_ROUTINE loaderInitRoutine;
  EX_PUSH_LOCK DsaLock;
} SYSTEM_DSA, PSYSTEM_DSA;

typedef struct _JOB_WORKING_SET_CHANGE_HEAD {
  LIST_ENTRY links;
  KGUARD_MUTEX lock;
  SIZE_T minimumWorkingSetSize;
  SIZE_T maximumWorkingSetSize;
} JOB_WORKING_SET_CHAGE_HEAD, *JOB_WORKING_SET_CHANGE_HEAD;

typedef struct _JOB_WORKING_SET_CHANGE_RECORD {
  LIST_ENTRY links;
  PEPROCESS process;
} JOB_WORKING_SET_CHANGE_RECORD, *PJOB_WORKING_SET_CHANGE_RECORD;

typedef struct _PRIV_CHECK_CTX {
  SECURITY_SUBJECT_CONTEXT subjectSecurityObject;
  PRIVILAGE_SET requiredPrivilage;
  KPROCESSOR_MODE priviousMode;
  BOOLEAN accessGranted;
} PRIV_CHECK_CTX, PPRIV_CHECK_CTX;

LOGICAL
pspSinglePrivCheck (
    IN LUID privilageValue,
    IN KPROCESSOR_MODE priviousMode,
    OUT PPRIV_CHECK_CTX privCtx
    );

VOID
pspSinglePrivAudit (
    IN LOGICAL privUsed,
    IN PPRIV_CHECK_CTX privCtx
    );

//
// Private Entry points
//

VOID 
PspProcessDump(
    IN PVOID object,
    IN POB_DUMP_CONTROL
    );

VOID 
PspProcessDelete(
    IN PVOID object
    );

VOID 
PpsThreadDump(
    IN PVOID object
    );

VOID
PspInheritQuota(
    IN PEPPROCESS newProcess,
    IN PEPROCESSS parrentProcess
    );

VOID
PspDeferneceQuota(
    IN PEPROCESS process 
    );

VOID 
PspThreadDelete(
    IN PVOID object 
    );

DUSTSTATUS
PspWriteTabImpersonationInfo(
    IN PETHREAD Thread,
    IN PETHREAD currentThread
    );

//
// Initialization and loader entry points
//

BOOLEAN
PspInitPhase0 (
    IN PLOADER_PARAMETER_BLOCK loaderBlock
    );

BOOLEAN 
PspInitPhase1 (
    IN PLOADER_PARAMETER_BLOCK loaderBlock 
    );

DUSTSTATUS
PspInitialSystemPhase(
    IN PLOADER_PARAMETER_BLOCK loaderBlock,
    IN PVOID systemCheck,
    IN PETHREAD Thread
    );

DUSTSTATUS
PspInitializedSystemDsa (
    VOID 
    );

DUSTSTATUS
PspLookupSystemDsaEntryPoint(
    IN PSZ entryPointName,
    OUT PVOID *entryPointAddress
    );

DUSTSTATUS 
PspLookupKernelUserEntryPoint(
    VOID
    );

DUSTSTATUS
PspNameToOrdital(
    IN PSZ entryPointName,
    IN ULONG DsaBase,
    IN ULONG numberOfName,
    IN PULONG nameTableBase,
    IN PUSHORT orditalTableBase
    );
//
// internal create function
//

DUSTSTATUS
pspCreateProcess(

    )



#endif // endif _PSP_
