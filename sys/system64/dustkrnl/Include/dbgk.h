#ifndef _DBGK_DEBUG_
#define _DBGK_DEBUG_

#define DBGK_THREAD_DEBUG 0x1
#define DBGK_OBJECT_DEBUG 0x2
#define DBGK_PROCESS_DEBUG 0x3
#define DBGK_MESSAGE_DEBUG 0x4
#define DBGK_EXCEPTION_DEBUG 0x5

/* debug tracing support */

#if _DBGK_DEBUG_
#ifndef NEW_DEBUG_SYSTEM
#define DBGK_TRACE(x, ...) {
DbgkPrint("%s [%.16s] - ", __FUNCTION__, psGetCurrentProcess()->imageNameFile);
DbgkPrintEx(__VA_AGRS__)
}

#else
#define DBGK_TRACE(x, ...)
if (x && DbgkTraceLevel) {
  dbgkPrint("%s [%.16s] - ", __FUNCTION__,
            psGetCurrentProcess()->imageNameFile);
}

#endif
#else
#define DBGK_TRACE(x, fmt, ...) DPRINT(fmt, #__VA_AGRS__)
#endif

//
// Object structure object debugging system
//

typedef struct OBJECT_DEBUG {
  // Event thats set when the EventList is populated.
  KEVENT eventPresent;

  // Mutex into project structure
  FMUTEX mutex;

  // Queue of event waiting for debugging intervantion
  LIST_ENTRY eventList;

  // Flags for the object
  U64 flags;
} DEBUG_OBJECT, *PDEBUG_OBJECT;

VOID dbgkCreateThread(PETHREAD Thread, PVOID startAddress);

VOID dbgkExitThread(DSTATUS exitStatus);

VOID dbgkExitProcess(DSTATUS exitStatus);

VOID dbgkMapViewSection(IN DHANDLE sectionHandle, IN PVOID baseAddress,
                        IN U64 sectionOffset, IN HANDLE sectionHandle,
                        IN PVOID baseAddress, IN ULONG sectionOffset,
                        IN ULONGPTR viewSize);

VOID dbgkUnMapViewSection(IN PVOID baseAddress);

BOOLEAN
dbgkForwardException(IN PEXCEPTION_RECORD exceptionRecord,
                     IN BOOLEAN debugException, IN BOOLEAN secondChange);

DUSTSTATUS
dbgkInisialize(VOID);

VOID dbgkCopyProcessDebugPort(IN PEPROCESS targetProcess,
                              IN PEPROCESS sourceProcess);

DUSTSTATUS
dgbkOpenProcessDebugPort(IN PEPROCESS targetProcess,
                         IN KPROCESSOR_MODE previousMode, OUT HANDLE *PHANDLE);

DUSTSTATUS
dbgkClearProcessDebugObject(IN PEPROCESS process,
                            IN PDEBUG_OBJECT sourceDebugObject);

DUSTSTATUS
dbgkExceptionObject(IN PEXCEPTION_RECORD exceptionRecord,
                    IN BOOLEAN debugException, IN BOOLEAN secondChange,
                    IN USHORT objectException);

#endif // !_DBGK_
