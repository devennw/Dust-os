#ifndef _DUSTOS_H_
#define _DUSTOS_H_

#include "bugcodes.h"
#include "dtosdef.h"
#include "exboosts.h"
#include "exlevels.h"
#include "init.h"
#include "procpowr.h"
#include "v86emul.h"
#include <dustkrnl.h>
#include <dustrtl.h>

#if defined(__x86_64__)
#include "x64.h"

#elif defined(__aarch64__)
#include "arm64.h"

#elif defined(__i386__)
#include "i386.h"

#else
#error "Architecture is not supported by Dust os"
#endif // amd64

#include "arc.h"
#include "cache.h"
#include "cm.h"
#include "dbgk.h"
#include "ex.h"
#include "hal.h"
#include "intrlk.h"
#include "io.h"
#include "kd.h"
#include "ke.h"
#include "kx.h"
#include "lfs.h"
#include "lpc.h"
#include "mm.h"
#include "ob.h"
#include "perf.h"
#include "pnp.h"
#include "po.h"
#include "ps.h"
#include "schad.h"
#include "se.h"
#include "syscall.h"
#include "verifier.h"
#include "wmi.h"

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