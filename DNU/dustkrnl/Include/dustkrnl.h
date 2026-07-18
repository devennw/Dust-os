#ifndef _DUSTKRNL_H_
#define _DUSTKRNL_H_

/* Primitive Fixed-Width Integers */

typedef unsigned char   U8;
typedef signed char     I8;

typedef unsigned short  U16;
typedef signed short    I16;

typedef unsigned int    U32;
typedef signed int      I32;

typedef unsigned long   U64;
typedef signed long     I64;

/* Memory and Architecture types (khusus x64) */

typedef unsigned long long  USIZE;
typedef unsigned long long  UINTPTR;
typedef void*               PTR_T;

/* Kernel System Types (Gaya ntoskrnl) */

typedef I32             dstatus_t;
typedef unsigned char   bool_t;
typedef void*           dhandle_t;

/* Penanda Nilai Boolean */

#define TRUE  1
#define FALSE 0

/* Penanda status dasar kernel */

#define DUST_SUCCESS          ((dstatus_t)0x00000000L)
#define DUST_ERROR_UNSUCCESS  ((dstatus_t)0xC0000001L)
#define DUST_ERROR_NO_MEMORY  ((dstatus_t)0xC0000017L)

//========================================================
// STRUCTUR FOR KERNEL FUNCTION
//========================================================

/* Sturctur string kustom */

typedef struct _DUST_STRING {
  U16 length;
  U16 maximum_length;
  char* buffer;
} DUST_STRING_T;

/* Structur antrean dua arah (Doubly linked list) */

typedef struct _DUST_LIST_ENTRY {
  struct _dust_list_entry*  next;
  struct _dust_list_entry* prev;
} DUST_LIST_ENTRY;

/* Structur manipulasi angka raksasa / waktu */

typedef union _DUST_LARGE_INT {
  struct {
    U32 low_part;
    I32 high_part;
  } split;
  I64 quad_part;
} DUST_LARGE_INT_T;

/* MODERN MULTICORE & VIRTUAL MEMORY TYPES */

typedef struct _DUST_SPINLOCK {
  vilatile U32 lock;
} DUST_SPINLOCK_T;

/* Structur identitas unik 128-Bit (Standar UEFI) */

typedef struct _DUST_GUID {
  U32 data1;
  U16 data2;
  U16 data3;
  U8 data4[8];
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

#endif // _DUSTKRNL_H_
