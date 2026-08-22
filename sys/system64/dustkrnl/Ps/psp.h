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
#define PS_ALIGN_DOWN(address, amt) ((uint32_t)(address) & ~((amt) - 1))

#define PS_ALIGN_UP(address, amt)                                              \
  (((uint32_t)(address) + ((amt) - 1)) & ~((amt) - 1))

#if defined(_AMD64_)

__forceinline
  PK_TRAP_FRAME PspGetTrapFrame(IN PETHREAD thread) {
  uint64_t initialStack;
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
  *void section;
  *void DsaBase;
  *void DllBase;
  PKNORMAL_ROUTINE loaderNormalInitRoutine;
  EX_PUSH_LOCK DsaLock;
  EX_PUSH_LOCK DllLock;
} SYSTEM_DSA, *PSYSTEM_DSA;

typedef struct _JOB_WORKING_SET_CHANGE_HEAD {
  LIST_ENTRY links;
  KGUARD_MUTEX lock;
  size_t minimumWorkingSetSize;
  size_t maximumWorkingSetSize;
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
  uint32_t PrivilegeCount;
  uint8_t AccessGranted;
} PRIVILAGE_CHECK_CONTEXT, *PPRIVILAGE_CHECK_CONTEXT;

LOGICAL
PspCheckPrivilege(LUID PrivilegeVl, KPROCESSOR_MODE PreviousMode,
                  PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext);

void
PspSinglePrivilegeCheckAudit(LOGICAL privUsed,
                             PPRIVILAGE_CHECK_CONTEXT PrivilegeCheckContext);

// Private Entry Point for Object Dumping
void
PspProcessDump(*void object, POB_DUMP_CONTROL control);

void
PspProcessDelete(*void object);

void
PspProcessDeleteDump(void);

void
PspThreadProcessDump(*void object, POB_DUMP_CONTROL control,
                     PETHREAD thread);

void
PspInheritQuotaLimits(PEPROCESS newProcess, PEPROCESS parentProcess);

void
PspDeferenceQuotaLimits(PEPROCESS process);

void
PspThreadDelete(*void object, PETHREAD objectThread);

int32_t
PspWriteTabImpersonationInfo(PETHREAD thread, PEPROCESS process,
                             *void impersonationInfo,
                             uint32_t impersonationInfoSize,
                             uint32_t bytesWritten);

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

void
PspThreadUserStartup(PKSTART_ROUTINE startRoutine,
                     *void startContext, PETHREAD threadStartup,
                     int32_t userStartup);

int32_t
PspThreadRunning(PETHREAD thread, *void threadHandle,
                 int32_t systemStartup, PETHREAD *threadPointer);

/* thread exit support */
void
PspExitApcRundownThread(PKAPC apc);

void
PspExitThreadSystem(int32_t exitStatus);

void
PspThreadTerminateByPointer(PETHREAD thread, int32_t exitStatus,
                            uint8_t DirectTerminate);

/* system inialize runtime */
void
PspRuntimeInialize(*void priviousMode, *void runtimeHandle,
                   int32_t systemStartup, PETHREAD thread,
                   uint8_t createdSuspended);

void
PspSystemRuntime(uint32_t runtimeStart, *void runtimeHandle);

/* context menegement */
void
PspContextSet(PKTRAP_FRAME trapFrame, *void context,
              CONTEXT_POINTERS contextNonVolatile,
              CONTEXT_POINTERS contextVolatile, PCONTEXT context,
              KPROCESSOR_MODE previousMode);

void
PspGetContext(PKTRAP_FRAME trapframe,
              PKNONVOLATILE_CONTEXT contextNonVolatile,
              PCONTEXT context,
              KPROCESSOR_MODE previousMode);

void
PspSetGetSpesialContextApc(PKAPC apc,
                           PKSTART_ROUTINE startRoutine,
                           PKNORMAL_ROUTINE *normalRoutine,
                           *void *normalContext,
                           *void *systemArgument1,
                           *void *systemArgument2);

void PspExitNormalRoutineApc(*void NormalContext, *void systemArgument1,
                             *void systemArgument2,
                             int32_t exitStatus);

/* private security routine */
int32_t
PspInializedSecurityProcess(PEPROCESS child, PEPROCESS parent);

void
PspDeleteSecurityProcess(PEPROCESS process);

void
PspInializedThreadProcess(PETHREAD thread, PEPROCESS process);

void
PspDeleteThreadProcess(PETHREAD thread);

int32_t
PspTokenPrimaryAssign(PEPROCESS process, *void tokenHandle,
                      *void token, PACCES_TOKEN tokenPointer);

int32_t
PspSetTokenPrimary(*void processHandle, PEPROCESS processPointer,
                   *void tokenHandle, *void token,
                   PACCES_TOKEN tokenPointer, uint8_t privilageChacked);

/* ltd support routine */
#if defined(i386)
int32_t PspLtdSupportRoutine();
#endif

/* Vdm support routine */
#if defined(i386)
int32_t PspVdmSupportRoutine();
#endif

int32_t
PspQueryLtdInformation(PEPROCESS process, *void ltdInformation,
                       uint32_t lengthLtdInformation, *uint32_t returnLength);

int32_t
PspSetLtdQuery(PEPROCESS process, *void ltdQuery,
               uint32_t LtdQueryLength);

int32_t
PspSetLtdInformation(PEPROCESS process, *void ltdInformation,
                     uint32_t lengthLtdInformation);

int32_t
PspSetLtdSize(PEPROCESS process, *void ltdsSize,
              uint32_t LtdSizeLength);

void
PspDeleteProcessLtd(PEPROCESS process);

/* Io handleing support routine */
int32_t
PspSetProcessHandleIo(PEPROCESS process, *void ioHandleInformation,
                      uint32_t HandleIoLength);

void
PspDeleteObjectVdm(PEPROCESS process);

int32_t
PspDescriptorThreadQuery(PEPROCESS process, PETHREAD thread,
                         *void threadInformation, uint32_t threadInformationLength,
                         *uint32_t returnLength);

/* object dir support routine */
int32_t
PspSetProcessObjectDir(PEPROCESS process, *void objectDirSize,
                       uint32_t objectDirSizeLength, *void objectHandle);

int32_t
PspDescriptorObjectQuery(PEPROCESS process, *void object,
                         *void objectDirInformation,
                         uint32_t objectDirInformationLength,
                         *uint32_t returnObjectLength);

VOID PspDeleteProcessObject(PEPROCESS process);

/* Job object support routine */




/* global data */
extern PHANDLE_TABLE PspCidTable;
extern *void PspInitialSystemProcessHandle;
extern PACCESS_TOKEN PspBootAccessToken;
extern SYSTEM_DLL PspSystemDll;
extern SYSTEM_DSA PspSystemDsa;
extern PETHREAD PspShutdownThread;

extern uint32_t PspDefaultPagedLimit;
extern uint32_t PspDefaultNonPagedLimit;
extern uint32_t PspDefaultPagefileLimit;
extern uint32_t PsMinimumWorkingSet;

extern EPROCESS_QUOTA_BLOCK PspDefaultQuotaBlock;
extern uint8_t PspDoingGiveBacks;

extern PKDUST64_PROCESS_CALLOUT PspD64ProcessCallout;
extern PKDUST64_THREAD_CALLOUT PspD64ThreadCallout;
extern PKDUST64_JOB_CALLOUT PspD64JobCallout;
extern uint32_t PspD64ProcessSize;
extern uint32_t PspD64ThreadSize;
extern int8_t PspForegroundQuantum[3];

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
