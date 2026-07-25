#ifndef _DUSTKRNL_H_
#define _DUSTKRNL_H_

#include <stdint.h>

//============================================
// 1. MACRO ANNOTATION AND SYSTEM DEFINITIONS
//============================================

#define DAPI
#define DUSTAPI

#define IN
#define OUT
#define OPTIONAL

#define FALSE   0
#define TRUE    1

#define ANYSIZE_ARRAY   1

#ifndef NULL
#define NULL ((void*)0)
#endif

//============================================
// 2. NT-STYLE PRIMITIVE TYPES AND POINTERS
//============================================

typedef void          VOID;
typedef void*         PVOID;
typedef const void*   PCVOID;

typedef unsigned long long  ULONGPTR;

typedef unsigned long long  U64;
typedef unsigned int        U32;
typedef unsigned short      U16;
typedef unsigned char       U8;

typedef signed long         I64;
typedef signed int          I32;
typedef signed short        I16;
typedef signed char         I8;

typedef char                S8;
typedef short               S16;
typedef int                 S32;
typedef long long           S64;

typedef uint64_t            ULONGLONG;
typedef uint32_t            ULONG;
typedef uint16_t            USHORT;
typedef uint8_t             UCHAR;

typedef int64_t             ILONG64;
typedef int32_t             IINT;
typedef int16_t             ISHORT;
typedef int8_t              ICHAR;


typedef unsigned char       BOOLEAN;

// Handle and status

typedef PVOID               HANDLE;
typedef PVOID               DHANDLE;
typedef I32                 DSTATUS;

//============================================
// 3. KERNEL STATUS RETURN CODES
//============================================

#define DSTATUS_SUCCESS                ((DSTATUS)0x00000000L)
#define DSTATUS_UNSECCESFUL            ((DSTATUS)0xC0000001L)
#define DSTATUS_NOT_IMPLEMETED         ((DSTATUS)0xC0000002L)
#define DSTATUS_INVALID_PARAMETER      ((DSTATUS)0xC000000DL)
#define DSTATUS_ACCESS_DENIED          ((DSTATUS)0x00000022l)
#define DSTATUS_INSFULLCIENT_RESOURCES ((DSTATUS)0x0000009Al)

//============================================
// 4. FORWARD DUST OS STRUCTURES
//============================================

typedef struct _EXCEPTION_RECORD {
  U32 exceptionCode;
  U32 exceptionFlags;
  struct _EXCEPTION_RECORD* ExceptionRecord;
  PVOID exceptionAddres;
  U32 numberParameters;
  ULONGPTR exceptionInformation[15];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

struct _EPROCESS;
typedef struct _EPROCESS EPROCESS, *PEPROCESS ;

struct _ETHREAD;
typedef struct _ETHREAD ETHREAD, *PETHREAD;

//=============================================
// 5. MEMORY SET OPTIMIZED DUST OS
//=============================================

#define offsetof(x,y) FIELD_OFFSET(x,y)
#define nil (0)

#define nelem(arr) (sizeof((arr)[0]) / sizeof((arr)))

int readn(void *fd, char *buf, U64 len);
int seek(void *fd, U64 off, int mode);

void *AllocMemZero(U64 num, U64 size);
void *Realloc(void *mem, U64 newsize);
void xfree(void *v);

#define werrstr(str, ...) DPRINT(str "\n" ,##__VA_ARGS__)
#define malloc(x) AllocMem(x)
#define mallocz(x,y) AllocMemZero(x,y)
#define free(x) xfree(x)
#define USED(x) (*((char *)&(x)) ^= 0)
#define memset(x,y,z) RtlZeroMemory(x,z)

//==============================================
// 6. ADVANCE DUST OS STRUCTURES
//==============================================

typedef struct _DUST_STR {
  U16 length;
  U16 maximumLength;
char* buffer;
} DUST_STR;

typedef struct _LIST_ENTRY {
  struct _LIST_ENTRY* next;
  struct _LIST_ENTRY* prev;
} LIST_ENTRY;

typedef union _LARGE_INTEGER {
  struct {
    U32 lowPart;
    I32 highPart;
  };
  I64 quadPart;
} LARGE_INTEGER;

typedef struct _DUST_SPINLOCK {
  volatile U32 lock;
} DUST_SPINLOCK;

typedef struct _DUST_GUID {
  U32 data1;
  U16 data2;
  U16 data3;
  U8 data4[8];
} DUST_GUID;

#endif // !_DUSTKRNL_H_
