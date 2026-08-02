#ifndef _DUSTKRNL_H_
#define _DUSTKRNL_H_

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

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

typedef void VOID, *PVOID, *LPVOID;
typedef char CHAR, CCHAR, *PCHAR, *PSTR, *LPSTR;
typedef const char *PCSTR, *LPCSTR;
typedef unsigned char UCHAR, *PUCHAR, BYTE, *LPBYTE, BOOLEAN, *PBOOLEAN;
typedef uint8_t UINT8;
typedef int16_t SHORT, *PSHORT;
typedef uint16_t USHORT, *PUSHORT, WORD, *PWORD, *LPWORD, WCHAR, *PWCHAR, *PWSTR, *LPWSTR, UINT16;
typedef const uint16_t *PCWSTR, *LPCWSTR;
typedef int32_t INT, LONG, *PLONG, *LPLONG, BOOL, WINBOOL, INT32;
typedef uint32_t UINT, *PUINT, *LPUINT, ULONG, *PULONG, DWORD, *PDWORD, *LPDWORD, UINT32;
#if defined(_DLP64) || defined(_DUST64)
typedef int64_t LONGPTR, *PLONGPTR, INTPTR, *PINTPTR;
typedef uint64_t ULONGPTR, DWORDPTR, *PULONGPTR, UINTPTR, *PUINTPTR;
#else
typedef int32_t LONGPTR, *PLONGPTR, INTPTR, *PINTPTR;
typedef uint32_t ULONGPTR, DWORDPTR, *PULONGPTR, UINTPTR, *PUINTPTR;
#endif
typedef uint64_t ULONG64, DWORD64, *PDWORD64, UINT64, ULONGLONG, ULONGPTR;
typedef int64_t LONGLONG, LONG64, INT64;
typedef float FLOAT;
typedef double DOUBLE;


// Handle and status

typedef PVOID HANDLE;
#ifndef _HAVE_HKEY
typedef HANDLE HKEY, *PHKEY;
#endif
typedef HANDLE HMODULE, HINSTANCE;
typedef INT NTSTATUS, POOL_TYPE;
typedef LONG HRESULT;
typedef ULONGPTR SIZE_T, *PSIZE_T;
typedef WORD LANGID;

#define MAXUSHORT USHRT_MAX

//============================================
// 3. KERNEL STATUS RETURN CODES
//============================================

#define DSTATUS_SUCCESS                ((DSTATUS)0x00000000L)
#define DSTATUS_UNSECCESFUL            ((DSTATUS)0xC0000001L)
#define DSTATUS_NOT_IMPLEMETED         ((DSTATUS)0xC0000002L)
#define DSTATUS_INVALID_PARAMETER      ((DSTATUS)0xC000000DL)
#define DSTATUS_ACCESS_DENIED          ((DSTATUS)0x00000022L)
#define DSTATUS_INSFULLCIENT_RESOURCES ((DSTATUS)0x0000009AL)

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

int readn(void *fd, char *buf, ULONG len);
int seek(void *fd, ULONG off, int mode);

void *AllocMemZero(ULONG num, ULONG size);
void *Realloc(void *mem, ULONG newsize);
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

#ifndef _HAVE_RTL_BITMAP
typedef struct _RTL_BITMAP
{
    ULONG  SizeOfBitMap;
    PULONG  Buffer;
} RTL_BITMAP, *PRTL_BITMAP;

typedef struct _RTL_BITMAP_RUN
{
    ULONG StartingIndex;
    ULONG NumberOfBits;
} RTL_BITMAP_RUN, *PRTL_BITMAP_RUN;
#endif

#ifndef _HAVE_LARGE_INTEGER
typedef union _LARGE_INTEGER
{
    struct
    {
        ULONG LowPart;
        LONG HighPart;
    };
    struct
    {
        ULONG LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
#endif

#ifndef _HAVE_LIST_ENTRY
typedef struct _LIST_ENTRY
{
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
} LIST_ENTRY,*PLIST_ENTRY;
#endif

#ifndef _HAVE_ANSI_STRING
typedef struct _ANSI_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PSTR   Buffer;
} ANSI_STRING, *PANSI_STRING;

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
#endif


#ifndef _HAVE_LIST_ENTRY
/* List Functions */
static __inline
VOID
InitializeListHead(
                   IN PLIST_ENTRY ListHead
                   )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

static __inline
VOID
InsertHeadList(
               IN PLIST_ENTRY ListHead,
               IN PLIST_ENTRY Entry
               )
{
    PLIST_ENTRY OldFlink;
    OldFlink = ListHead->Flink;
    Entry->Flink = OldFlink;
    Entry->Blink = ListHead;
    OldFlink->Blink = Entry;
    ListHead->Flink = Entry;
}

static __inline
VOID
InsertTailList(
               IN PLIST_ENTRY ListHead,
               IN PLIST_ENTRY Entry
               )
{
    PLIST_ENTRY OldBlink;
    OldBlink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = OldBlink;
    OldBlink->Flink = Entry;
    ListHead->Blink = Entry;
}

static __inline
BOOLEAN
IsListEmpty(
            IN const LIST_ENTRY * ListHead
            )
{
    return (BOOLEAN)(ListHead->Flink == ListHead);
}

static __inline
BOOLEAN
RemoveEntryList(
                IN PLIST_ENTRY Entry)
{
    PLIST_ENTRY OldFlink;
    PLIST_ENTRY OldBlink;

    OldFlink = Entry->Flink;
    OldBlink = Entry->Blink;
    OldFlink->Blink = OldBlink;
    OldBlink->Flink = OldFlink;
    return (BOOLEAN)(OldFlink == OldBlink);
}

static __inline
PLIST_ENTRY
RemoveHeadList(
               IN PLIST_ENTRY ListHead)
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

static __inline
PLIST_ENTRY
RemoveTailList(
               IN PLIST_ENTRY ListHead)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}
#endif

#ifndef _HAVE_ANSI_STRING
typedef const UNICODE_STRING *PCUNICODE_STRING;
#endif

/* Widely used macros */
#define LOBYTE(w)               ((BYTE)(w))
#define HIBYTE(w)               ((BYTE)(((WORD)(w)>>8)&0xFF))
#define LOWORD(l)               ((WORD)((DWORD_PTR)(l)))
#define HIWORD(l)               ((WORD)(((DWORD_PTR)(l)>>16)&0xFFFF))
#define MAKEWORD(a,b)           ((WORD)(((BYTE)(a))|(((WORD)((BYTE)(b)))<<8)))
#define MAKELONG(a,b)           ((LONG)(((WORD)(a))|(((DWORD)((WORD)(b)))<<16)))

#define MAXULONG 0xFFFFFFFF

#define NT_SUCCESS(x)           ((x)>=0)
#if !defined(__GNUC__)
#define FIELD_OFFSET(t,f)       ((LONG)(LONG_PTR)&(((t*) 0)->f))
#else
#define FIELD_OFFSET(t,f)       ((LONG)__builtin_offsetof(t,f))
#endif
#define RTL_CONSTANT_STRING(s)  { sizeof(s)-sizeof((s)[0]), sizeof(s), s }
#define CONTAINING_RECORD(address, type, field)  ((type *)(((ULONG_PTR)address) - (ULONG_PTR)(&(((type *)0)->field))))

#define RtlZeroMemory(Destination, Length)            memset(Destination, 0, Length)
#define RtlCopyMemory(Destination, Source, Length)    memcpy(Destination, Source, Length)
#define RtlMoveMemory(Destination, Source, Length)    memmove(Destination, Source, Length)

#define MAKELANGID(p,s)         ((((WORD)(s))<<10)|(WORD)(p))
#define PRIMARYLANGID(l)        ((WORD)(l)&0x3ff)
#define SUBLANGID(l)            ((WORD)(l)>>10)
#define SUBLANG_NEUTRAL         0x00

/* Prevent inclusion of some other headers */
#define __INTERNAL_DEBUG
#define RTL_H

#endif // !_DUSTKRNL_H_
