#ifndef __amd64_
#define __amd64_

#if defined (_M_X64)

void
KeCompactServiceTable (
    void* table;
    uint32_t limit;
    uint8_t Dust32k;
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

uint8_t
__inbyte (uint16_t port);

uint16_t
__inword (uint16_t port);

uint32_t
__inlong (uint16_t port);

void
__outbyte (uint16_t port);

void
__outword (uint16_t port);

void
__outlong (uint16_t port);



#ifndef __cplusplus
}
#endif
