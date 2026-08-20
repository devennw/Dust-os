// Module Name:

//     pshelper.c

// Abstract:

//     EPROCESS and ETHREAD field access for DUSTOS-external components

#include "psp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PsIsProcessBeingDebugged)
#pragma alloc_text(PAGE, PsIsThreadImpersonating)
#pragma alloc_text(PAGE, PsReferenceProcessFilePointer)
#pragma alloc_text(PAGE, PsSetProcessDust32Process)
#pragma alloc_text(PAGE, PsSetProcessSecurityPort)
#pragma alloc_text(PAGE, PsSetJobUIRestrictionsClass)
#pragma alloc_text(PAGE, PsSetProcessWindowStation)
#pragma alloc_text(PAGE, PsGetProcessSecurityPort)
#pragma alloc_text(PAGE, PsSetThreadDust32Thread)
#pragma alloc_text(PAGE, PsGetProcessExitProcessCalled)
#pragma alloc_text(PAGE, PsGetThreadSessionId)
#pragma alloc_text(PAGE, PsSetProcessPriorityClass)
#pragma alloc_text(PAGE, PsIsSystemProcess)
#endif

/*++
--*/
#undef PsGetCurrentProcess
PEPROCESS
PsGetCurrentProcess(VOID) { 
    return _PsGetCurrentProcess(); 
}

ULONG
PsGetCurrentSessionId(VOID) { 
    return mmGetSessionId(_PsGetSessionId()); 
}

ULONG
PsGetCurrentDetectId(VOID) { 
    return dtkDetectIdSession(_PsGetDetectId()); 
}

#undef PsGetThread
PETHREAD
PsGetThread(VOID) {
    return _PsGetThread();
}

PVOID
PsGetThreadStackBase(VOID) {
    return _PsGetThreadStackBase(); 
}

VOID PsGetThreadStackLimits(VOID) {
  return _PsGetThreadStackLimits()->Stacklimits;
}

ULONG
PsGetThreadStackId(VOID) {
    return thrGetThread(_PsGetThreadStackId());
}

CCHAR
PsGetThreadPreviousMode(VOID) { 
    return KePsGetPreviousMode(); 
}

PERESOURCE
PsGetLockJob(__in PEJOB job) { 
    return &job->jobLock; 
}

ULONG
PsSessionJobId(__in PEJOB job) {
    return &job->sessionId; 
}

ULONG
PsGetJobUIRestrictionClassJob(__in PEJOB job) {
  return job->UIRestictiationClass;
}

LONGLONG
PsGetProcessCreateTimeQuadPart(__in PEPROCESS process) {
  return process->crateTime.QuadPart;
}

#undef PsObjectProcess
#ifndef __cplusplus
{
#endif

  PEPROCESS
  PsObjectDirFunc(VOID) { 
      return _PsObjectDirFunc()->objectRun; 
  }

  VOID PsHelpingObDirCount(IN PVOID countObDir, IN PULONG helpingObject, ) {
    return _PsHelpingObDirCount()->createTime.QuadPart;
  }

  DUSTSTATUS
  PsHelperByObject(VOID) { 
      return _PsHelperByObject()->sessionId; 
  }

  ULONG
  PsGetCurrentHelpingOnOb(VOID) {
    return _PsGetCurrentHelpingOnOb()->objectRun;
  }

  ULONG
  PsGetObjectRunning(VOID) { 
      return _PsGetObjectRunning();
  }

  DUSTSTATUS
  PsStatusObjectRun(__in PETHREAD thread, __out PVOID infoObject) {
    return _PsStatusObjectRun()->sessionId;
  }

  DUSTSTATUS
  PsExitProcessObjectDir(__in PVOID infoObject, __in DUSTSTATUS exitStatus) {
    return exitProcessObjectRun(_PsObjectDirFunc()->objectRun);
  }

#ifndef __cplusplus
}
#endif // __cplusplus

#undef PsGetDebug
PEPROCESS
PsGetProcessDebug(VOID) { 
    return _PsGetProcessDebug(); 
}

DUSTSTATUS
PsGetDebugInfo(VOID) {
    return _PsGetDebugInfo()->sessionId; 
}

PVOID
PsGetProcessDebugPort(__in PEPROCESS process) {
    return process->DebugPort;
}

VOID PsGetProcessDebugObjectDir(IN PEPROCESS process) {
  return _PsDebugObjectDir()->objectRun;
}

BOOLEAN
PsProcessIsBeingDebugged(__in PEPROCESS process) {
  if (process->DebugPort != NULL) {
    return TRUE;
  } else {
    return FALSE;
  }

BOOLEAN
PsGetExitProcessCalled(__in PEPROCESS process) {
  return (BOOLEAN)((process->flags & PS_FLAGS_PROCESS_EXITING) != 0);
}

BOOLEAN
PsExitDebugProcess(__in DUSTSTATUS exitStatus) {
  return exitProcess(_PsGetProcessDebug()->exitProcess);
}

DUSTSTATUS
PsExitObjectDebugging(__in DUSTSTATUS exitStatus, __in PEPROCESS process,
                      __in PETHREAD thread OPTIONAL) {
  return ExitProcessObjectDebug(_PsDebugObjectDir()->objectRun);
}

HANDLE
PsGetProcessId(__in PEPROCESS process) {
    return process->uniqueProcessId; 
}

UCHAR
*PspGetProcessImageNameFile(__in PEPROCESS process) {
    return process->imageFileName;
}

HANDLE
PspGetProcessUniqueInheritProcessId(__in PEPROCESS process) {
    return process->uniqueInheritProcessId;
}

PEJOB 
PsGetProcessJob(__in PEPROCESS process) {
    return process->job;
}

ULONG
PsGetProcessSessionId(__in PEPROCESS process) {
    return mmGetSessionId(process);
}

ULONG
PsGetProcessSessionIdEx(__in PEPROCESS process) {
    return mmGetSessionIdEx(process);
}

PVOID
PsGetProcessSectionBaseAddress(__in PEPROCESS) {
    return process->sectionBaseAddress;
}

PPEB PsGetProcessPeb(__in PEPROCESS process) {
    return process->peb;
}

UCHAR
PsGetProcessClassPriority(__in PEPROCESS process) {
    return process->classPriority;
}

HANDLE
PsGetProcessDust64WindowStation(__in PEPROCESS process) {
    return process->dust64WindowStation;
}

PVOID
PsGetProcessDust64Process(__in PEPROCESS process) {
    return process->dust64Process;
}

HANDLE
PsGetThreadId(__in PETHREAD thread) {
    return thread->cId.UniqueThread;
}

CCHAR
PsGetThreadFreezeCount(__in PETHREAD thread) {
    return thread->tcb.freezeCount;
}

BOOLEAN
PsGetDisabledForThreadHardError(__in PETHREAD thread) {
    return (BOOLEAN) (thread->);
}

PEPROCESS
PsGetThreadProcess(__in PETHREAD thread) {
    return PROCESS_THREAD(thread);
}

PEPROCESS 
PsGetCurrentThreadProcess(VOID) {
    return PROCESS_THREAD(_PsGetCurrentProcess());
}