#ifndef _PSP_
#define _PSP_

#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 4214) // nonstandard extension used : bit field types other than int
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
#pragma warning(disable: 4514) // unreferenced inline function has been removed
#pragma warning(disable: 4702) // unreachable code
#pragma warning(disable: 4820) // padding added after data member

#pragma warning(disable: 4054)   // Cast of function pointer to PVOID
#pragma warning(disable: 4055)   // Cast of function pointer
#pragma warning(disable: 4115)   // named type definition in parentheses
#pragma warning(disable: 4127)   // condition expression is constant
#pragma warning(disable: 4152)   // Casting function pointers
#pragma warning(disable: 4327)   // alignment on assignment
#pragma warning(disable: 4328)   // alignment on assignment

#include <dustos.h>
#include <dustrtl.h>
#include "dusturtl.h"
#include "zwapi.h"
#include "ki.h"

#if defined(_X86_)
#include <dustvdm.h>
#endif

#if defined(_AMD64_)
#include <dustvdm_amd64.h>
#endif

#if defined(_ARM64_)
#include <dustvdm_arm64.h>
#endif

//
// working set watcher is 16 kb. this lets us have a 4k page
// for the working set watcher and 12k for the stack.
//
#define WS_CATCH_SIZE 12288
#define WS_OVERHEAD 12
#define MAX_CATCH_WS_INDEX (((WS_CATCH_SIZE - WS_OVERHEAD)/sizeof    \
                           (PROCESS_WATCH_WS_INFORMATION))-2 )

//
// Process Quota Charges:
//
//  PagedPool
//      Directory Base Page - PAGE_SIZE
//
//  NonPaged
//      Object Body         - sizeof(EPROCESS)
//

#define PROCESS_QUOTA_CHARGE_PAGED_POOL (PAGE_SIZE)
#define PROCESS_QUOTA_CHARGE_NONPAGED_POOL (sizeof(EPROCESS))

//
// Thread Quota Charges:
//
//  PagedPool
//      Kernel Stack        - 0
//
//  NonPaged
//      Object Body         - sizeof(ETHREAD)
//

#define PS_THREAD_QUOTA_CHARGE_PAGED_POOL (0)
#define PS_THREAD_QUOTA_CHARGE_NONPAGED_POOL (sizeof(ETHREAD))

//
// Define routines to get trap and exception frame addresses
//

#define PS_ALIGN_DOWN(address, amt) ((ULONG_PTR)(address) & ~((amt) - 1))

#define PS_ALIGN_UP(address, amt) (((ULONG_PTR)(address) + ((amt) - 1)) &   \
                                                           ~((amt) - 1))

#if defined(_AMD64_)

FORCEINLINE
PK_TRAP_FRAME
PspGetTrapFrame(
    IN PETHREAD thread
    )
{
    ULONGLONG initialStack;
    PKERNEL_STACK_CONTROL stackControl;

    initialStack = (ULONGLONG)thread->InitialStack;
    stackControl = (PKERNEL_STACK_CONTROL)initialStack;
    do {
        initialStack = stackControl->Previous.initialStack;
        stackControl = (PKERNEL_STACK_CONTROL)initialStack;
    } while (stackControl->Previous.stackBase != NULL);

    return (PK_TRAP_FRAME)(initialStack + sizeof(KERNEL_STACK_CONTROL));
}

#define PspGetExceptionFrame(thread) ((PK_EXC_FRAME)(PspGetTrapFrame(thread) + 1))

#elif defined(_X86_)

#define PspGetBaseTrapFrame(thread) (PKTRAP_FRAME)((ULONG_PTR)Thread->Tcb.initialStack - \
                                                   PSPALIGN_UP(sizeof(KTRAP_FRAME),KTRAP_FRAME_ALIGN) - \
                                                   sizeof(FX_SAVE_AREA))
#define PspGetBaseExceptionFrame(thread)    (NULL)

#elif defined(_ARM64_)

#define PspGetBaseTrapFrame(thread) (PKTRAP_FRAME)((ULONG_PTR)Thread->Tcb.initialStack - \
                                                   PSPALIGN_UP(sizeof(KTRAP_FRAME),KTRAP_FRAME_ALIGN) - \
                                                   sizeof(FX_SAVE_AREA))

#define PspGetBaseExceptionFrame(thread)    (NULL)

#else
#error "Unsupported architecture for Dust"
#endif // defined(_AMD64_)

typedef struct _GET_SET_CONTEXT {
    KAPC Apc;
    KPROCESSOR_MODE mode;
    KEVENT operationCompleteEvent;
    CONTEXT context;
    KNONVOLATILE_CONTEXT_POINTERS nonVolatileContextPointers;
} GET_SET_CONTEXT, *PGET_SET_CONTEXT;

typedef struct _SYSTEM_DSA {
    PVOID section;
    PVOID DsaBase;
    PKNORMAL_ROUTINE loaderNormalInitRoutine;
    EX_PUSH_LOCK dsaLock;
} SYSTEM_DSA, *PSYSTEM_DSA;

typedef struct _JOB_WORKING_SET_CHANGE_HEAD {
    LIST_ENTRY links;
    KGUARD_MUTEX lock;
    SIZE_T minimumWorkingSetSize;
    SIZE_T maximumWorkingSetSize;
} JOB_WORKING_SET_CHANGE_HEAD, *PJOB_WORKING_SET_CHANGE_HEAD;

typedef struct _JOB_WORKING_SET_CHANGE_RECORD {
    LIST_ENTRY links;
    PEPROCESS process;
} JOB_WORKING_SET_CHANGE_RECORD, *PJOB_WORKING_SET_CHANGE_RECORD;
JOB_WORKING_SET_CHANGE_HEAD* PspGetJobWorkingSetChangeHead(PEPROCESS Process);

typedef struct _PRIVILAGE_CHECK_CONTEXT {
    SECURITY_SUBJECT_CONTEXT SubjectContext;
    PEPROCESS Process;
    PRIVILEGE_SET requiredPrivileges;
    PETHREAD Thread;
    KPROCESSOR_MODE PreviousMode;
    ULONG PrivilegeCount;
    BOOLEAN AccessGranted;
} PRIVILAGE_CHECK_CONTEXT, *PPRIVILAGE_CHECK_CONTEXT;

LOGICAL
PspCheckPrivilege(
  IN LUID PrivilegeVl,
  IN KPROCESSOR_MODE PreviousMode,
  OUT PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext
  );

VOID
PspSinglePrivilegeCheckAudit (
  IN LOGICAL privUsed,
  IN PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext
  );

// Private Entry Point for Object Dumping

VOID
PspProcessDump (
  IN PVOID object,
  IN POB_DUMP_CONTROL control OPTIONAL
  );

VOID
PspProcessDelete (
  IN PVOID object
  );

VOID
PspProcessDeleteDump (
  VOID
  );

VOID
PspThreadProcessDump (
  IN PVOID object,
  IN POB_DUMP_CONTROL control OPTIONAL,
  IN PETHREAD thread OPTIONAL
    );

VOID
PspInheritQuotaLimits (
  IN PEPROCESS newProcess,
  IN PEPROCESS parentProcess
  );

VOID
PspDeferenceQuotaLimits (
  IN PEPROCESS process
  );

VOID
PspThreadDelete (
  IN PVOID object,
  IN PETHREAD objectThread
    );

DUSTSTATUS
PspWriteTabImpersonationInfo (
  IN PETHREAD thread,
  IN PEPROCESS process,
  IN PVOID impersonationInfo,
  IN ULONG impersonationInfoSize,
  OUT ULONGPTR bytesWritten
  );

// Initialization loader entry point for the process and thread subsystems

DUSTSTATUS
PspInitializeProcessSubsystem (
  IN PVOID dsaBase
  );

VOID
PspInitializeThreadSubsystem (
  IN PVOID dsaBase,
  IN PETHREAD thread
  );

ULONG
PspGetProcessSessionId (
  IN PEPROCESS process,
  OUT ULONGPTR sessionId
  );

ULONG
PspGetProcessSessionIdEx (
  IN PEPROCESS process,
  );

// initialization and loader enrty point

BOOLEAN
PspLoaderInitializeProcess (
  IN PLOADER_PARAMETER_BLOCK LoaderBlock,
  IN PEPROCESS process,
  IN PVOID pebAddress,
  IN PVOID *processParameters,
  );

BOOLEAN
PspLoaderInitPhase0 (
  IN PLOADER_PARAMETER_BLOCK loaderBlock
  );

BOOLEAN
PspLoaderInitPhase1 (
  IN PLOADER_PARAMETER_BLOCK loaderBlock
  );

DUSTSTATUS
PspInitializeSystemDsa (
  VOID
  );

DUSTSTATUS
PspInitiasizeSystemDsaEntryPoint (
  IN PSZ entryPointName,
  IN PVOID *entryPointAddress
  );

DUSTSTATUS
PspLookupKernelEntryPoint (
  VOID
  );

USHORT
PspNameForOrdinal (
  IN PSZ entryPoint,
  IN ULONG dsaBase,
  IN ULONG numberToName,
  IN PULONG nameTableBase,
  IN PUSHORT ordinalNumber
  );

#endif // _PSP_
