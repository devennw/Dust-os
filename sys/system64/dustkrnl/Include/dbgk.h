#ifndef _DBGK_DEBUG_
#define _DBGK_DEBUG_

#include <cstdint>
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
  uint32_t eventPresent;

  // Mutex into project structure
  int32_t mutex;

  // Queue of event waiting for debugging intervantion
  LIST_ENTRY eventList;

  // Flags for the object
  uint64_t flags;
} DEBUG_OBJECT, *PDEBUG_OBJECT;

void 
dbgkCreateThread(PETHREAD Thread, void* startAddress);

void
dbgkExitThread(int32_t exitStatus);

void
dbgkExitProcess(int32_t exitStatus);

void
dbgkMapViewSection(void* sectionHandle, void* baseAddress,
                   uint64_t sectionOffset, void* sectionHandle,
                   void* baseAddress, uint32_t sectionOffset,
                   uint32_t viewSize);

void
dbgkUnMapViewSection(void* baseAddress);

uint8_t
dbgkForwardException(PEXCEPTION_RECORD exceptionRecord,
                     uint8_t debugException, uint8_t secondChange);

int32_t
dbgkInisialize(void);

void 
dbgkCopyProcessDebugPort(PEPROCESS targetProcess,
                         PEPROCESS sourceProcess);

int32_t
dgbkOpenProcessDebugPort(PEPROCESS targetProcess,
                         KPROCESSOR_MODE previousMode, void void*);

int32_t
dbgkClearProcessDebugObject(PEPROCESS process,
                            PDEBUG_OBJECT sourceDebugObject);

int32_t
dbgkExceptionObject(PEXCEPTION_RECORD exceptionRecord,
                    uint8_t debugException, uint8_t secondChange,
                    uint16_t objectException);

#endif // !_DBGK_
