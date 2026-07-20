#ifndef _DBGK_
#define _DBGK_

#define DBGK_THREAD     0x1
#define DBGK_OBJECT     0x2
#define DBGK_PROCESS    0x3
#define DBGK_MESSAGE    0x4
#define DBGK_EXCEPTION  0x5

/* debug tracing support */

#if _DBGK_
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
      DbgkPrint("%s [%.16s] - ",
          __FUNCTION__,
          psGetCurrentProcess()-> imageNameFile);
    }

#endif
#else
#define DBGK_TRACE(x, fmt, ...) DPRINT (fmt, #__VA_AGRS__)
#endif

#endif // !_DBGK_
