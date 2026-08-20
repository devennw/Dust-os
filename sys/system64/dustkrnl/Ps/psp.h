#ifndef _PSP_
#define _PSP_

#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable:4214) // nonstandard extension used : bit field types other than int
#pragma warning(disable:4324) // structure was padded due to alignment specifier
#pragma warning(disable:4514) // unreferenced inline function has been removed
#pragma warning(disable:4702) // unreachable code
#pragma warning(disable:4820) // padding added after data member

#pragma warning(disable:4054) // Cast of function pointer to PVOID
#pragma warning(disable:4055) // Cast of function pointer
#pragma warning(disable:4115) // named type definition in parentheses
#pragma warning(disable:4127) // condition expression is constant
#pragma warning(disable:4152) // Casting function pointers
#pragma warning(disable:4327) // alignment on assignment
#pragma warning(disable:4328) // alignment on assignment

#include "dusturtl.h"
#include "ki.h"
#include "zwapi.h"
#include <dustos.h>
#include <dustrtl.h>

#if defined(_X86_)
#include <dustvdm.h>
#endif

#if defined(_AMD64_)
#include <dustvdm_amd64.h>
#endif

#if defined(_ARM64_)
#include <dustvdm_arm64.h>
#endif

#define NOAPIEXT
#include "ddbgext.h"
#include "dustdbg.h"
#include <string.h>

//
// working set watcher is 16 kb. this lets us have a 4k page
// for the working set watcher and 12k for the stack.
//
#define WS_CATCH_SIZE 12288
#define WS_OVERHEAD 12
#define MAX_CATCH_WS_INDEX                                                     \
  (((WS_CATCH_SIZE - WS_OVERHEAD) / sizeof(PROCESS_WATCH_WS_INFORMATION)) - 2)

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

#define PS_ALIGN_UP(address, amt)                                              \
  (((ULONG_PTR)(address) + ((amt) - 1)) & ~((amt) - 1))

#if defined(_AMD64_)

__forceinline
  PK_TRAP_FRAME PspGetTrapFrame(IN PETHREAD thread) {
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

#define PspGetExceptionFrame(thread)                                           \
  ((PK_EXC_FRAME)(PspGetTrapFrame(thread) + 1))

#elif defined(_X86_)

#define PspGetBaseTrapFrame(thread)                                            \
  (PKTRAP_FRAME)((ULONG_PTR)Thread->Tcb.initialStack -                         \
                 PSPALIGN_UP(sizeof(KTRAP_FRAME), KTRAP_FRAME_ALIGN) -         \
                 sizeof(FX_SAVE_AREA))
#define PspGetBaseExceptionFrame(thread) (NULL)

#elif defined(_ARM64_)

#define PspGetBaseTrapFrame(thread)                                            \
  (PKTRAP_FRAME)((ULONG_PTR)Thread->Tcb.initialStack -                         \
                 PSPALIGN_UP(sizeof(KTRAP_FRAME), KTRAP_FRAME_ALIGN) -         \
                 sizeof(FX_SAVE_AREA))

#define PspGetBaseExceptionFrame(thread) (NULL)

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
  PVOID DllBase;
  PKNORMAL_ROUTINE loaderNormalInitRoutine;
  EX_PUSH_LOCK DsaLock;
  EX_PUSH_LOCK DllLock;
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
JOB_WORKING_SET_CHANGE_HEAD *PspGetJobWorkingSetChangeHead(PEPROCESS Process);

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
PspCheckPrivilege(IN LUID PrivilegeVl, IN KPROCESSOR_MODE PreviousMode,
                  OUT PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext);

VOID PspSinglePrivilegeCheckAudit(
    IN LOGICAL privUsed, IN PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext);

// Private Entry Point for Object Dumping

VOID PspProcessDump(IN PVOID object, IN POB_DUMP_CONTROL control OPTIONAL);

VOID PspProcessDelete(IN PVOID object);

VOID PspProcessDeleteDump(VOID);

VOID PspThreadProcessDump(IN PVOID object, IN POB_DUMP_CONTROL control OPTIONAL,
                          IN PETHREAD thread OPTIONAL);

VOID PspInheritQuotaLimits(IN PEPROCESS newProcess, IN PEPROCESS parentProcess);

VOID PspDeferenceQuotaLimits(IN PEPROCESS process);

VOID PspThreadDelete(IN PVOID object, IN PETHREAD objectThread);

DUSTSTATUS
PspWriteTabImpersonationInfo(IN PETHREAD thread, IN PEPROCESS process,
                             IN PVOID impersonationInfo,
                             IN ULONG impersonationInfoSize,
                             OUT ULONGPTR bytesWritten);

// Initialization loader entry point for the process and thread subsystems

DUSTSTATUS
PspInitializeProcessSubsystem(IN PVOID dsaBase);

VOID PspInitializeThreadSubsystem(IN PVOID dsaBase, IN PETHREAD thread);

ULONG
PspGetProcessSessionId(IN PEPROCESS process, OUT ULONGPTR sessionId);

ULONG
PspGetProcessSessionIdEx(IN PEPROCESS process, );

// initialization and loader enrty point

BOOLEAN
PspLoaderInitializeProcess(IN PLOADER_PARAMETER_BLOCK LoaderBlock,
                           IN PEPROCESS process, IN PVOID pebAddress,
                           IN PVOID *processParameters, );

BOOLEAN
PspLoaderInitPhase0(IN PLOADER_PARAMETER_BLOCK loaderBlock);

BOOLEAN
PspLoaderInitPhase1(IN PLOADER_PARAMETER_BLOCK loaderBlock);

DUSTSTATUS
PspInitializeSystemDsa(VOID);

DUSTSTATUS
PspInitiasizeSystemDsaEntryPoint(IN PSZ entryPointName,
                                 IN PVOID *entryPointAddress);

DUSTSTATUS
PspLookupKernelEntryPoint(VOID);

USHORT
PspNameForOrdinal(IN PSZ entryPoint, IN ULONG dsaBase, IN ULONG numberToName,
                  IN PULONG nameTableBase, IN PUSHORT ordinalNumber);

USHORT
PspOrdinalEntryPoint(IN PVOID ordinalEntryPoint, IN PUSHORT ordinalNumber,
                     IN PULONG nameTableBase);

/* Internal Creation Function */

DUSTSTATUS
PspCreateProcess(OUT PHANDLE processHandle, IN ACCSES_MASK accsessDesired,
                 IN POBJECT_ATTRIBUTES objectAttributes,
                 IN HANDLE parentProcess, IN ULONG flags,
                 IN HANDLE sectionHandle OPTIONAL, IN HANDLE debugPort OPTIONAL,
                 IN HANDLE exceptionPort, IN ULONG jobMemberLevel);

#define PSP_CREATE_MAX_PROCESS_NOTIFY 12
#define PSP_CREATE_MIN_PROCESS_NOTIFY 6

//
// Define process callouts. These are of type PCREATE_PROCESS_NOTIFY_ROUTINE
// Called on process create and delete.
//
ULONG
PspCreateProcessNotifyRoutineCount;

EX_CALLBACK
PspCreateProcessNotifyRoutine[PSP_MAX_CREATE_PROCESS_NOTIFY];

#define PSP_LOAD_MAX_IMAGE_NOTIFY 12

//
// Define process callouts. These are of type PCREATE_PROCESS_NOTIFY_ROUTINE
// Called on process create and delete.
//
ULONG
PspCreateMinProcessNotifyRoutineCount;

EX_CALLBACK
PspCreateMinProcessNotifyRoutineCount[PSP_CREATE_MIN_PROCESS_NOTIFY]

#define PSP_LOAD_MIN_IMAGE_NOTIFY 6

    //
    // Define image load callbacks. These are of type PLOAD_IMAGE_NOTIFY_ROUTINE
    // Called on image load.
    //

    ULONG PspLoadImageNotifyRoutineCount;

ULONG
PspLoadMinImageNotifyRoutineCount;

EX_CALLBACK
PspLoadImageNotifyRoutine[PSP_LOAD_MAX_IMAGE_NOTIFY];

EX_CALLBACK
PspLoadMinImageNotifyRoutine[PSP_LOAD_MIN_IMAGE_NOTIFY];

DUSTSTATUS
PspCreatethread(OUT PHANDLE ThreadHandle, IN ACCESS_MASK DesiredAccess,
                IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
                IN HANDLE ProcessHandle, IN PEPROCESS ProcessPointer,
                OUT PCLIENT_ID ClientId OPTIONAL, IN PCONTEXT ThreadContext,
                IN PINITIAL_TEB InitialTeb OPTIONAL, IN BOOLEAN CreateSuspended,
                IN PKSTART_ROUTINE StartRoutine OPTIONAL, IN PVOID StartContext)

    /* starup routine */

    VOID
    PspTheadUserStarup(IN PKSTART_ROUTINE startRoutine, IN PVOID startContext);

VOID PspThreadSystemStartup(IN PKSTART_ROUTINE startRoutine,
                            IN PVOID startContext);

VOID PspReaper(IN PVOID startContext);

/*++
--*/

VOID PspThreadStartup(OUT HANDLE threadProcess, IN PETHREAD thread,
                      IN PVOID startContext, IN PKSTART_ROUTINE startRoutine,
                      IN PCONTEXT threadContext OPTIONAL,
                      IN ULONG threadStart OPTIONAL);

VOID PspThreadUserStartup(IN PKSTART_ROUTINE startRoutine,
                          IN PVOID startContext, IN PETHREAD threadStartup,
                          IN DUSTSTATUS userStartup);

DUSTSTATUS
PspThreadRunning(IN PETHREAD thread, IN PHANDLE threadHandle,
                 IN DUSTSTATUS systemStartup,
                 OUT PETHREAD *threadPointer OPTIONAL);

/* thread exit support */

VOID PspExitApcRundownThread(IN PKAPC apc);

VOID PspExitThreadSystem(IN DUSTSTATUS exitStatus);

VOID PspThreadTerminateByPointer(IN PETHREAD thread, IN DUSTSTATUS exitStatus,
                                 IN BOOLEAN DirectTerminate);

/* system inialize runtime */

VOID PspRuntimeInialize(IN PVOID priviousMode, IN HANLDE runtimeHandle,
                        OUT DUSTSTATUS systemStartup OPTIONAL,
                        IN PETHREAD thread, IN BOOLEAN createdSuspended);

VOID PspSystemRuntime(IN ULONG runtimeStart, IN HANDLE runtimeHandle);

/* context menegement */

VOID PspContextSet(OUT PKTRAP_FRAME trapFrame, IN PVOID context,
                   IN CONTEXT_POINTERS contextNonVolatile,
                   IN CONTEXT_POINTERS contextVolatile, IN PCONTEXT context,
                   IN KPROCESSOR_MODE previousMode);

VOID PspGetContext(IN PKTRAP_FRAME trapframe,
                   IN PKNONVOLATILE_CONTEXT contextNonVolatile,
                   IN OUT PCONTEXT context,
                   IN OUT KPROCESSOR_MODE previousMode OPTIONAL);

VOID PspSetGetSpesialContextApc(IN PKAPC apc,
                                IN PKSTART_ROUTINE startRoutine OPTIONAL,
                                IN OUT PKNORMAL_ROUTINE *normalRoutine,
                                IN OUT PVOID *normalContext,
                                IN OUT PVOID *systemArgument1,
                                IN OUT PVOID *systemArgument2);

VOID PspExitNormalRoutineApc(IN PVOID NormalContext, IN PVOID systemArgument1,
                             IN PVOID systemArgument2,
                             IN DUSTSTATUS exitStatus);

/* private security routine */

DUSTSTATUS
PspInializedSecurityProcess(IN PEPROCESS child, IN PEPROCESS parent OPTIONAL);

VOID PspDeleteSecurityProcess(IN PEPROCESS process);

VOID PspInializedThreadProcess(IN PETHREAD thread, IN PEPROCESS process);

VOID PspDeleteThreadProcess(IN PETHREAD thread);

DUSTSTATUS
PspTokenPrimaryAssign(IN PEPROCESS process, IN PHANDLE tokenHandle,
                      IN HANDLE token, IN PACCES_TOKEN tokenPointer OPTIONAL);

DUSTSTATUS
PspSetTokenPrimary(IN HANDLE processHandle,
                   IN PEPROCESS processPointer OPTIONAL, IN PHANDLE tokenHandle,
                   IN HANDLE token, IN PACCES_TOKEN tokenPointer OPTIONAL,
                   IN BOOLEAN privilageChacked)

/* ltd support routine */

#if defined(i386)
DUSTSTATUS PspLtdSupportRoutine();
#endif

/* Vdm support routine */

#if defined(i386)
DUSTSTATUS PspVdmSupportRoutine();
#endif

DUSTSTATUS
PspQueryLtdInformation(IN PEPROCESS process, OUT PVOID ltdInformation,
                       IN ULONG lengthLtdInformation, OUT PULONG returnLength);

DUSTSTATUS
PspSetLtdInformation(IN PEPROCESS process, IN PVOID ltdInformation,
                     IN ULONG lengthLtdInformation);

DUSTSTATUS
PspSetLtdSize(IN PEPROCESS process, IN PVOID ltdsSize,
              IN ULONG lengthLtdSize);

DUSTSTATUS
PspSetLtdQuery(IN PEPROCESS process, IN PVOID ltdQuery,
               IN ULONG lengthLtdQuery);

VOID
PspDeleteProcessLtd(IN PEPROCESS process);





/* global data */

extern PHANDLE_TABLE PspCidTable;
extern HANDLE PspInitialSystemProcessHandle;
extern PACCESS_TOKEN PspBootAccessToken;
extern SYSTEM_DLL PspSystemDsa;
extern PETHREAD PspShutdownThread;

extern ULONG PspDefaultPagedLimit;
extern ULONG PspDefaultNonPagedLimit;
extern ULONG PspDefaultPagefileLimit;
extern ULONG PsMinimumWorkingSet;

extern EPROCESS_QUOTA_BLOCK PspDefaultQuotaBlock;
extern BOOLEAN PspDoingGiveBacks;

extern PKDUST64_PROCESS_CALLOUT PspD64ProcessCallout;
extern PKDUST64_THREAD_CALLOUT PspD64ThreadCallout;
extern PKDUST64_JOB_CALLOUT PspD64JobCallout;
extern ULONG PspD64ProcessSize;
extern ULONG PspD64ThreadSize;
extern SCHAR PspForegroundQuantum[3];

#define PSP_NUMBER_OF_SCHEDULING_CLASSES 16
#define PSP_DEFAULT_SCHEDULING_CLASSES 8

extern const SCHAR PspJobSchedulingClasses[PSP_NUMBER_OF_SCHEDULING_CLASSES];
extern BOOLEAN PspUseJobSchedulingClasses;

extern LIST_ENTRY PspJobList;
extern KDPC PspJobLimeLimitsDpc;
extern KTIMER PspJobTimeLimitsTimer;
extern WORK_QUEUE_ITEM PspJobTimeLimitsWorkItem;
extern KSPIN_LOCK PspQuotaLock;

#endif // _PSP_
