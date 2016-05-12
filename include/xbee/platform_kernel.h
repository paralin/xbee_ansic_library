/*
 * Copyright (c) 2010-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

/**
  @addtogroup hal_kernel
  @{
  @file xbee/platform_kernel.h
  Header for kernel modules/code (Linux).

  This file is automatically included by xbee/platform.h.
*/

#ifndef __XBEE_PLATFORM_KERNEL
#define __XBEE_PLATFORM_KERNEL
  #include <linux/string.h>

  #define strcmpi    strcasecmp
  #define strncmpi    strncasecmp

  // Load platform's endian header to learn whether we're big or little.
  #include <linux/types.h>
  #include <linux/tty.h>

  typedef uint8_t uint_fast8_t;
  typedef uint8_t uint8_fast8_t;

  #define LITTLE_ENDIAN 1
  #define BIG_ENDIAN 0
  #ifdef __LITTLE_ENDIAN
  #define BYTE_ORDER LITTLE_ENDIAN
  #elif defined(__BIG_ENDIAN)
  #define BYTE_ORDER BIG_ENDIAN
  #else
  #error Unknown endianness.
  #endif

  // macro used to declare a packed structure (no alignment of elements)
  #define PACKED_STRUCT    struct __attribute__ ((__packed__))

  #define _f_memcpy    memcpy
  #define _f_memset    memset

  typedef int bool_t;

  // compiler natively supports 64-bit integers
  #define XBEE_NATIVE_64BIT

// Elements needed to keep track of serial port settings.  Must have a
// baudrate memember, other fields are platform-specific.
typedef struct xbee_serial_t {
  uint32_t    baudrate;
  struct tty_struct* tty;
  struct tty_ldisc_ops* ldisc_ops;
} xbee_serial_t;

// Unix epoch is 1/1/1970
#define ZCL_TIME_EPOCH_DELTA  ZCL_TIME_EPOCH_DELTA_1970

// Per POSIX standard, "The resolution of the system clock is unspecified."
// We assume the millisecond timer has at least a 10ms resolution.
#define XBEE_MS_TIMER_RESOLUTION 10

#endif    // __XBEE_PLATFORM_KERNEL
