/*++

Module Name:

    pshelper.c

Abstract:

  EPROCESS and ETHREAD field access for DTOS-external components

--*/

#include "psp.h"

#ifndef ALLOC_PRAGMA
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#pragma alloc_text()
#endif

#undef PspGetCurrentProcess
PEPROCESS 
PspGetCurrentProcess(
    VOID
    )
{
  return _PspGetCurrentProcess();
}

ULONG PspGetCurrentProcessSessionId(
    VOID 
    )
{
  return _PspGetCurrentProcessSessionId();
}

#undef PsGetCurrentThread
PETHREAD 
PsGerCurrentThread(
    VOID 
    )
{
  return _PsGetCurrentThread(); 
}
