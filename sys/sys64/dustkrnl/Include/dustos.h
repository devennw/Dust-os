#ifndef _DUSTOS_H_
#define _DUSTOS_H_

#include <dustkrnl.h>
#include <dustrtl.h>
#include "dtosdef.h"
#include "exlevels.h"
#include "exboosts.h"
#include "bugcodes.h"
#include "init.h"
#include "v86emul.h"
#include "procpowr.h"

#if defined(__x86_64__)
#include "x64.h"

#elif defined(__aarch64__)
#include "arm64.h"

#elif defined(__i386__)
#include "i386.h"

#else
#error "Architecture is not supported by Dust os"
#endif // amd64

#include "intrlk.h"
#include "arc.h"
#include "ke.h"
#include "kd.h"
#include "ex.h"
#include "ps.h"
#include "se.h"
#include "io.h"
#include "ob.h"
#include "mm.h"
#include "lpc.h"
#include "dbgk.h"
#include "lfs.h"
#include "cache.h"
#include "pnp.h"
#include "hal.h"
#include "kx.h"
#include "cm.h"
#include "po.h"
#include "perf.h"
#include "schad.h"
#include "syscall.h"
#include "wmi.h"
#include "verifier.h"

#define _DUSTDDK_

/* Temp. Until we define a header file for types
 * Outside of the kernel these are exported by reference */

#ifdef _DUSTDRIVER_ 
extern POBJECT_TYPE *ExEventPairObjectTypes;
extern POBJECT_TYPE *PsProcessTypes;
extern POBJECT_TYPE *PsThreadTypes;
extern POBJECT_TYPE *PsJobTypes;
extern POBJECT_TYPE *LpcPortObjectTypes;
extern POBJECT_TYPE *LpcWaitablePortObjectTypes;

#else 

extern POBJECT_TYPE ExEventPairObjectTypes;
extern POBJECT_TYPE PsProcessTypes;
extern POBJECT_TYPE PsThreadTypes;
extern POBJECT_TYPE PsJobTypes;
extern POBJECT_TYPE LpcPortObjectTypes;
extern POBJECT_TYPE LpcWaitablePortObjectTypes;

#endif

#ifdef _DUSTDRIVER_
