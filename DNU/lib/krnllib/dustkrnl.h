#ifndef _DUSTKRNL_H_
#define _DUSTKRNL_H_

/* Primitive Fixed-Width Integers */

typedef unsigned char        U8;
typedef signed char          I8;

typedef unsigned short       U16;
typedef signed short         I16;

typedef unsigned int         U32;
typedef signed int           I32;

typedef unsigned long        U64;
typedef signed long          I64;

typedef short                S16;
typedef int                  S32;
typedef long long            LONG63;

/* Memory and Architecture types (khusus x64) */

typedef unsigned long long  ULONG64;
typedef unsigned long long  ULONGPTR;
typedef void*               PVOID;

/* Kernel System Types (Gaya ntoskrnl) */

typedef I32                 DSTATUS;
typedef unsigned char       BOOLEAN;
typedef void*               DHANDLE;

/* DNU set  */

#ifndef _DNU
typedef U64                 VOID;
typedef ULONG64             DUSTAPI;
#endif

/* Penanda Nilai Boolean */

#define TRUE  1
#define FALSE 0

/* Penanda status dasar kernel */

#define DUST_SUCCESS          ((dstatus_t)0x00000000F)
#define DUST_ERROR_UNSUCCESS  ((dstatus_t)0xC0000001L)
#define DUST_ERROR_NO_MEMORY  ((dstatus_t)0xC0000017L)

//========================================================
// STRUCTUR FOR KERNEL FUNCTION
//========================================================

/* Sturctur string kustom */

typedef struct _DUST_STRING {
  U16   length;
  U16   maximum_length;
  char* buffer;
} DUST_STRING_T;

/* Structur antrean dua arah (Doubly linked list) */

typedef struct _DUST_LIST_ENTRY {
  struct _dust_list_entry*   next;
  struct _dust_list_entry*   prev;
} DUST_LIST_ENTRY_T;

/* Structur manipulasi angka raksasa / waktu */

typedef union _DUST_LARGE_INT {
  struct {
    U32   low_part;
    I32   high_part;
  } split;
  I64     quad_part;
} DUST_LARGE_INT_T;

/* MODERN MULTICORE & VIRTUAL MEMORY TYPES */

typedef struct _DUST_SPINLOCK {
  VILATILE U32  lock;
} DUST_SPINLOCK_T;

/* Structur identitas unik 128-Bit (Standar UEFI) */

typedef struct _DUST_GUID {
  U32   data1;
  U16   data2;
  U16   data3;
  U8    data4[8];
} DUST_GUID_T;

/* Structur untuk menyimpan isi jantung CPU x64 saat multitasking */

typedef struct _DUST_CONTEXT {

  // Register Umum (Geneal purpuse register x64)
  U64 R15, R14, R13, R12, R11, R10, R9, R8;
  U64 rbp, rdi, rsi, rdx, rcx, rbx, rax;

  // Register khusus saat terjadi interupsi hardware
  U64 interrupt_number;
  U64 error_code;

  // Register otomatis yang dijatuhkan oleh silikon cpu x64
  U64 rip;
  U64 cs;
  U64 rflag;
  U64 rsp;
  U64 ss;
} DUST_CONTEXT_T;

typedef U64 DUST_PTE_T;
#define DUST_PTE_PRESENT  (1NULL << 0)
#define DUST_PTE_WRITABLE (1NULL << 1)
#define DUST_PTE_USER     (1NULL << 2)

/* Pointer set for memory allocator menegement */

#define offsetof(x,y) FIELD_OFFSET(x,y)
#define nil (0)

#define nelem(arr) (sizeof((arr)[0]) / sizeof((arr)))

int readn(void *fd, char *buf, U64 len);
int seek(void *fd, U64 off, int mode);

void *RosSymAllocMemZero(U64 num, U64 size);
void *RosSymRealloc(void *mem, U64 newsize);
void xfree(void *v);

#define werrstr(str, ...) DPRINT(str "\n" ,##__VA_ARGS__)
#define malloc(x)       DustSymAllocMem(x)
#define mallocz(x,y)    DustSymAllocMemZero(x,y)
#define free(x)         xfree(x)
#define USED(x)         (*((char *)&(x)) ^= 0)
#define memset(x,y,z)   RtlZeroMemory(x,z)

#endif // _DUSTKRNL_H_
