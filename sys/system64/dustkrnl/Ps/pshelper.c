// Module Name:

//     pshelper.c

// Abstract:

//     EPROCESS and ETHREAD field access for NTOS-external components


#include "psp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, PsIsProcessBeingDebugged)
#pragma alloc_text (PAGE, PsIsThreadImpersonating)
#pragma alloc_text (PAGE, PsReferenceProcessFilePointer)
#pragma alloc_text (PAGE, PsSetProcessDust32Process)
#pragma alloc_text (PAGE, PsSetProcessSecurityPort)
#pragma alloc_text (PAGE, PsSetJobUIRestrictionsClass)
#pragma alloc_text (PAGE, PsSetProcessWindowStation)
#pragma alloc_text (PAGE, PsGetProcessSecurityPort)
#pragma alloc_text (PAGE, PsSetThreadDust32Thread)
#pragma alloc_text (PAGE, PsGetProcessExitProcessCalled)
#pragma alloc_text (PAGE, PsGetThreadSessionId)
#pragma alloc_text (PAGE, PsSetProcessPriorityClass)
#pragma alloc_text (PAGE, PsIsSystemProcess)
#endif

/*++
--*/
#undef PsGetCurrentProcess
PEPROCESS
PsGetCurrentProcess (
  VOID
  )
{
  return _PsGetCurrentProcess();
}

ULONG
PsGetCurrentSessionId (
  VOID
  )
{
  return mmGetSessionId (_PsGetSessionId());
}

ULONG
PsGetCurrentDetectId (
  VOID
  )
{
  return dtkDetectIdSession (_PsGetDetectId());
}

#undef PsGetThread
PETHREAD
PsGetThread (
  VOID
  )
{
  return _PsGetThread();
}

PVOID
PsGetThreadStackBase (
  VOID
  )
{
  return _PsGetThreadStackBase();
}

VOID
PsGetThreadStackLimits (
  VOID
  )
{
  return _PsGetThreadStackLimits()->Stacklimits;
}

ULONG
PsGetThreadStackId (
  VOID
  )
{
  return thrGetThread(_PsGetThreadStackId(););
}

CCHAR
PsGetThreadPreviousMode (
  VOID
  )
{
  return KePsGetPreviousMode();
}

PERESOURCE
PsGetLockJob (
  __in PEJOB job
  )
{
  return &job->jobLock;
}

ULONG
PsSessionJobId (
  __in PEJOB job
  )
{
  return &job->sessionId;
}

ULONG
PsGetJobUIRestrictionClassJob (
  __in PEJOB job
  )
{
  return job->UIRestictiationClass;
}

LONGLONG
PsGetProcessCreateQuadTimePart(
    __in PEPROCESS process
    )
{
    return process->crateTime.QuadPart;
}

PVOID
PsGetProcessDebugPort(
    __in PEPROCESS process
    )
{
    return process->DebugPort;
}



#ifndef __cplusplus
{
#endif

PEPROCESS
PsObjectDirFunc (
  VOID
  )
{
  return _PsObjectDirFunc();
}

VOID
PsHelpingObDirCount (
  IN PVOID countObDir,
  IN PULONG helpingObject,
  )
{
  return _PsHelpingObDirCount();
}

DUSTSTATUS
PsHelperByObject (
  VOID
  )
{
  return _PsHelperByObject()->sessionId;
}

ULONG
PsGetCurrentHelpingOnOb (
  VOID
  )
{
  return _PsGetCurrentHelping();
}

ULONG
PsGetObjectRunning (
  VOID
  )
{
  return _PsGetObjectRunning();
}

DUSTSTATUS
PsStatusObjectRun (
  __in PETHREAD thread,
  __out PVOID infoObject
  )
{
  return _PsStatusObjectRun();
}

#ifndef __cplusplus
}
#endif
