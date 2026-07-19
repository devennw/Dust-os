#ifndef _DTOS_H
#define _DTOS_H

#include "dustkrnl.h"
#include "dustrtl.h"
#include "dtosdef.h"
#include "dustkrnl.h" 

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

#ifdef _DUSTDRIVER_ 

#ifdef _DUSTDRIVER_
