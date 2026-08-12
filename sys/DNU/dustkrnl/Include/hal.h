/*

Module Name:

    hal.h

Abstract:

    This header file defines the Hardware Architecture Layer (HAL) interfaces
    that are exported by a system vendor to the DUST system.

 */

#ifndef _HAL_SYS_
#define _HAL_SYS_

//
// Define OEM bitmapped font check values.
//

#define HAL_OEM_FONT_VERSION 0x200
#define HAL_OEM_FONT_TYPE 0
#define HAL_OEM_FONT_ITALIC 0
#define HAL_OEM_FONT_UNDERLINE 0
#define HAL_OEM_FONT_STRIKEOUT 0
#define HAL_OEM_FONT_CHARSET 0xff
#define HAL_OEM_FONT_FAMILY (4 << 5)

