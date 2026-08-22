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

  initialStack = (uint64_t)thread->InitialStack;
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
  (PKTRAP_FRAME)((uint32_t)Thread->Tcb.initialStack -                         \
                 PSPALIGN_UP(sizeof(KTRAP_FRAME), KTRAP_FRAME_ALIGN) -         \
                 sizeof(FX_SAVE_AREA))
#define PspGetBaseExceptionFrame(thread) (NULL)

#elif defined(_ARM64_)

#define PspGetBaseTrapFrame(thread)                                            \
  (PKTRAP_FRAME)((uint32_t)Thread->Tcb.initialStack -                         \
                 PSPALIGN_UP(sizeof(KTRAP_FRAME), KTRAP_FRAME_ALIGN) -         \
                 sizeof(FX_SAVE_AREA) + 2)

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
PspCheckPrivilege(LUID PrivilegeVl, KPROCESSOR_MODE PreviousMode,
                  PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext);

VOID PspSinglePrivilegeCheckAudit(
    IN LOGICAL privUsed, IN PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext);

// Private Entry Point for Object Dumping
VOID PspProcessDump(IN PVOID object, IN POB_DUMP_CONTROL control OPTIONAL);

VOID PspProcessDelete(IN PVOID object);

void
PspProcessDeleteDump(void);

VOID PspThreadProcessDump(IN PVOID object, IN POB_DUMP_CONTROL control OPTIONAL,
                          IN PETHREAD thread OPTIONAL);

VOID PspInheritQuotaLimits(IN PEPROCESS newProcess, IN PEPROCESS parentProcess);

VOID PspDeferenceQuotaLimits(IN PEPROCESS process);

void
PspThreadDelete(*void object, PETHREAD objectThread);

DUSTSTATUS
PspWriteTabImpersonationInfo(IN PETHREAD thread, IN PEPROCESS process,
                             IN PVOID impersonationInfo,
                             IN ULONG impersonationInfoSize,
                             OUT ULONGPTR bytesWritten);

// Initialization loader entry point for the process and thread subsystems
int32_t
PspInitializeProcessSubsystemDsa(*void dsaBase);

int32_t
PspInitializedProcessSubsystemDll(*void dllBase);

void
PspInitializeThreadSubsystem(*void dsaBase, *void dllBase,
                             PETHREAD thread);

uint32_t
PspGetProcessSessionId(PEPROCESS process, uint32_t sessionId);

uint32_t
PspGetProcessSessionIdEx(PEPROCESS process, uint32_t sessionIdEx);

// initialization and loader enrty point
uint8_t
PspLoaderInitializeProcess(PLOADER_PARAMETER_BLOCK LoaderBlock,
                           PEPROCESS process, *void pebAddress,
                           *void *processParameters);

uint8_t
PspLoaderInitPhase0(PLOADER_PARAMETER_BLOCK loaderBlock);

uint8_t
PspLoaderInitPhase1(PLOADER_PARAMETER_BLOCK loaderBlock);

int32_t
PspInitializeSystemDsa(void);

int32_t
PspInitiasizeSystemDsaEntryPoint(PSZ entryPointName,
                                 *void *entryPointAddress);

int32_t
PspLookupKernelEntryPoint(void);

uint16_t
PspNameForOrdinal(PSZ entryPoint, uint32_t dsaBase, uint32_t numberToName,
                  *uint32_t nameTableBase, *uint16_t ordinalNumber);

uint16_t
PspOrdinalEntryPoint(*void ordinalEntryPoint, *uint16_t ordinalNumber,
                     *uint32_t nameTableBase);

/* Internal Creation Function */
int32_t
PspCreateProcess(*void processHandle, ACCSES_MASK accsessDesired,
                 POBJECT_ATTRIBUTES objectAttributes,
                 *void parentProcess, uint32_t flags,
                 *void sectionHandle, *void debugPort,
                 *void exceptionPort, uint32_t jobMemberLevel);

#define PSP_CREATE_MAX_PROCESS_NOTIFY 12
#define PSP_CREATE_MIN_PROCESS_NOTIFY 6

//
// Define process callouts. These are of type PCREATE_PROCESS_NOTIFY_ROUTINE
// Called on process create and delete.
//
uint32_t
PspCreateProcessNotifyRoutineCount;

EX_CALLBACK
PspCreateProcessNotifyRoutine[PSP_MAX_CREATE_PROCESS_NOTIFY];

#define PSP_LOAD_MAX_IMAGE_NOTIFY 12

//
// Define process callouts. These are of type PCREATE_PROCESS_NOTIFY_ROUTINE
// Called on process create and delete.
//
uint32_t
PspCreateMinProcessNotifyRoutineCount;

EX_CALLBACK
PspCreateMinProcessNotifyRoutineCount[PSP_CREATE_MIN_PROCESS_NOTIFY]

#define PSP_LOAD_MIN_IMAGE_NOTIFY 6

//
// Define image load callbacks. These are of type PLOAD_IMAGE_NOTIFY_ROUTINE
// Called on image load.
//
uint32_t
PspLoadImageNotifyRoutineCount;

uint32_t
PspLoadMinImageNotifyRoutineCount;

EX_CALLBACK
PspLoadImageNotifyRoutine[PSP_LOAD_MAX_IMAGE_NOTIFY];

EX_CALLBACK
PspLoadMinImageNotifyRoutine[PSP_LOAD_MIN_IMAGE_NOTIFY];

int32_t
PspCreatethread(*void ThreadHandle, ACCESS_MASK DesiredAccess,
                POBJECT_ATTRIBUTES ObjectAttributes,
                *void ProcessHandle, PEPROCESS ProcessPointer,
                PCLIENT_ID ClientId , PCONTEXT ThreadContext,
                PINITIAL_TEB InitialTeb, uint8_t CreateSuspended,
                PKSTART_ROUTINE StartRoutine, *void StartContext)

/* starup routine */
void
PspTheadUserStarup(PKSTART_ROUTINE startRoutine,
                   *void startContext);

void
PspThreadSystemStartup(PKSTART_ROUTINE startRoutine,
                       *void startContext);

void
PspReaper(*void startContext);

/*++
--*/
void
PspThreadStartup(*void threadProcess, PETHREAD thread,
                 *void startContext, PKSTART_ROUTINE startRoutine,
                 PCONTEXT threadContext, uint32_t threadStart);

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
PspSetLtdQuery(IN PEPROCESS process, IN PVOID ltdQuery,
               IN ULONG LtdQueryLength);

DUSTSTATUS
PspSetLtdInformation(IN PEPROCESS process, IN PVOID ltdInformation,
                     IN ULONG lengthLtdInformation);

DUSTSTATUS
PspSetLtdSize(IN PEPROCESS process, IN PVOID ltdsSize,
              IN ULONG LtdSizeLength);

VOID
PspDeleteProcessLtd(IN PEPROCESS process);

/* Io handleing support routine */
DUSTSTATUS
PspSetProcessHandleIo(IN PEPROCESS process, IN PVOID ioHandleInformation,
    IN ULONG HandleIoLength);

VOID PspDeleteObjectVdm(IN PEPROCESS process);

DUSTSTATUS
PspDescriptorThreadQuery(IN PEPROCESS process, IN PETHREAD thread,
                         IN PVOID threadInformation ,IN ULONG threadInformationLength,
                         IN PULONG returnLength);

/* object dir support routine */
DUSTSTATUS
PspSetProcessObjectDir(IN PEPROCESS process, IN PVOID objectDirSize,
                       IN ULONG objectDirSizeLength,
                       IN HANDLE objectHandle);

DUSTSTATUS
PspDescriptorObjectQuery(IN PEPROCESS process, IN PVOID object,
                         IN PVOID objectDirInformation,
                         IN ULONG objectDirInformationLength,
                         IN PULONG returnObjectLength);

VOID PspDeleteProcessObject(IN PEPROCESS process);

/* Job object support routine */




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
