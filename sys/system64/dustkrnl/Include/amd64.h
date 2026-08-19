#ifndef __amd64_
#define __amd64_

#if defined (_M_X64)

VOID
KeCompactServiceTable (
    IN PRTV_T table;
    IN ULONG limit;
    IN BOOLEAN Dust32k;
    );

/* header mechine architecture */

#define IMAGE_HEADER_FILE_MECHINE_NATIVE  0xb4a8
#endif

/* end dustsp */

#if (!(defined(_DUST_DRIVER_)) || defined(_DUSTDDK_) || defined(_DUSTIFS_) \
    || defined(_DUSTHAL_) || defined(_DUSTSP_)) && !defined(_BLDR_)

#define exRaiseException    rtlRaiseException
#define exRaiseStatus       rtlRaiseStatus

#endif

/* mendefinisikan fungsi unik untuk keluar dan masuk */

#ifdef __cplusplus
extern "C" {
#endif

UCHAR
__inbyte (
    IN USHORT port
    );

USHORT
__inword (
    IN USHORT port
    );

ULONG
__inlong (
    IN USHORT port
    );

VOID
__outbyte (
    IN USHORT port
    );

VOID
__outword (
    IN USHORT port
)

VOID
__outlong (
    IN USHORT port
)



#ifndef __cplusplus
}
#endif
