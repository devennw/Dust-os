#ifndef _DTOSDEF_
#define _DTOSDEF_

typedef struct _EXCEPTION_RECORD {
  uint32_t exceptionCode;
  uint32_t exceptionFlags;
  struct _EXCEPTION_RECORD* ExceptionRecord;
  void* exceptionAddres;
  uint32_t numberParameters;
  uint32_t exceptionInformation[15];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

struct _EPROCESS;
typedef struct _EPROCESS EPROCESS, *PEPROCESS ;

struct _ETHREAD;
typedef struct _ETHREAD ETHREAD, *PETHREAD;

typedef struct _DUST_STR {
  uint16_t length;
  uint16_t maximumLength;
  uint8_t buffer;
  uint16_t length;
  uint16_t maximumLength;
  uint8_t buffer;
} DUST_STR;

typedef struct _LIST_ENTRY {
  struct _LIST_ENTRY* next;
  struct _LIST_ENTRY* prev;
} LIST_ENTRY;

typedef union _LARGE_INTEGER {
  struct {
    uint32_t lowPart;
    int32_t highPart;
  };
    int64_t quadPart;
    uint32_t lowPart;
    int32_t highPart;
  };
  int64_t quadPart;
} LARGE_INTEGER;

typedef struct _DUST_SPINLOCK {
  volatile uint32_t lock;
} DUST_SPINLOCK;

typedef struct _DUST_GUID {
  uint32_t data1;
  uint16_t data2;
  uint16_t data3;
  uint8_t data4[8];
} DUST_GUID;

#ifndef _HAVE_RTL_BITMAP
typedef struct _RTL_BITMAP
{
    uint32_t  SizeOfBitMap;
    uint32_t  Buffer;
} RTL_BITMAP, *PRTL_BITMAP;

typedef struct _RTL_BITMAP_RUN
{
    uint32_t StartingIndex;
    uint32_t NumberOfBits;
} RTL_BITMAP_RUN, *PRTL_BITMAP_RUN;
#endif

#ifndef _HAVE_LARGE_INTEGER
typedef union _LARGE_INTEGER
{
    struct
    {
        uint32_t LowPart;
        int32_t HighPart;
    };
    struct
    {
        uint32_t LowPart;
        int32_t HighPart;
    } u;
    int64_t QuadPart;
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
    uint16_t Length;
    uint16_t MaximumLength;
    uint32_t Buffer;
} ANSI_STRING, *PANSI_STRING;

typedef struct _UNICODE_STRING
{
    uint16_t Length;
    uint16_t MaximumLength;
    uint32_t Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
#endif


#ifndef _HAVE_LIST_ENTRY
/* List Functions */
static __inline
void
InitializeListHead(
                   IN PLIST_ENTRY ListHead
                   )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

static __inline
void
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
void
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
uint8_t
IsListEmpty(
            IN const LIST_ENTRY * ListHead
            )
{
    return (BOOLEAN)(ListHead->Flink == ListHead);
}

static __inline
uint8_t
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
RemoveTailLis(PLIST_ENTRY ListHead)
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


#endif