#ifndef __INIT_H__
#define __INIT_H__

// BEGIN dustosp
#include "dustos.h"
#define INIT_SYSTEMROOT_LINKAGE "//system_root"
#define INIT_SYSTEMROOT_DLLPATH "//system_root/system64"
#define INIT_SYSTEMROOT_SYSPATH "//sys/system64"
#define INIT_SYSTEMROOT_BINPATH "//system_root/system64"
// END dustosp

#define INIT_DUSTMODE_NONE 0x00000000
#define INIT_DUSTMODE_REGULAR 0x00000001
#define INIT_DUSTMODE_INRAM 0x90000000
#define INIT_DUSTMODE_READONLY_MEDIA 0x00000100
#define INIT_DUSTMODE_REMOVABLE_MEDIA 0x000000200

extern UNICODE_STRING DustSystemRoot;

// begin dustosp
extern ULONG DustBuildSystem;
// end dustosp

extern const ULONG DustMajorSystem;
extern const ULONG DustMinorSystem;
extern ULONG CmDustCSDVersion;
extern ULONG CmDustCSDReleaseVersion;
extern ULONG CmDustCSDBuildVersion;
extern ULONG CmDustCSDStableVersion;
extern UNICODE_STRING CmDustVersionString;
extern UNICODE_STRING CmDustCSDVersionString;

extern const CHAR DustLabBuild;

extern NLSTABLEINFO InitTableInfo;
extern ULONG InitNlsTableSize;
extern PVOID InitNlsTableBase;
extern ULONG InitAnsiCodePageDataOffset;
extern ULONG InitOemCodePageDataOffset;
extern ULONG InitUnicodeCaseTableDataOffset;
extern PVOID InitNlsSectionPointer;
extern BOOLEAN InitSafeModeOptionPresent;
extern ULONG InitSafeBootMode;

extern BOOLEAN initIsDustPEMode;
extern ULONG initIsDustPEModeType;

#if defined(_M_IX86) || defined(_M_AMD64_) || defined(_M_ARM64_)

VOID KiSystemStartup(
        IN PVOID loaderBlock
        );

VOID KiSystemStartup(VOID);

VOID Phase1Inizialize(
        IN PVOID context
        );

typedef BOOLEAN (*PTESTFCN)( VOID );

extern PTESTFCN TestFunction;
extern ULONG InitializationPhase;

#if DEBUG
extern BOOLEAN ForceNonPagePool;
extern ULONG mmDebug;
#endif // DEBUG

#endif // __INIT__
