#ifndef _DBGK_DEBUG_
#define _DBGK_DEBUG_

#define DBGK_THREAD_DEBUG     0x1
#define DBGK_OBJECT_DEBUG     0x2
#define DBGK_PROCESS_DEBUG    0x3
#define DBGK_MESSAGE_DEBUG    0x4
#define DBGK_EXCEPTION_DEBUG  0x5

/* debug tracing support */

#if _DBGK_DEBUG_
#ifndef NEW_DEBUG_SYSTEM
#define DBGK_TRACE(x, ...) {
      DbgkPrint("%s [%.16s] - ",
          __FUNCTION__,
          psGetCurrentProcess()-> imageNameFile);
      DbgkPrintEx(__VA_AGRS__)
    }

#else
#define DBGK_TRACE(x, ...)
    if (x && DbgkTraceLevel) {
      dbgkPrint("%s [%.16s] - ",
          __FUNCTION__,
          psGetCurrentProcess()-> imageNameFile);
    }

#endif
#else
#define DBGK_TRACE(x, fmt, ...) DPRINT (fmt, #__VA_AGRS__)
#endif


#endif // !_DBGK_
