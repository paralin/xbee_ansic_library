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
	@file kernel/xbee_readline.c

	ANSI C xbee_readline() implementation that works for kernel code

	Function documentation appears in xbee/platform.h.
*/

#include <linux/errno.h>
/*
#include <linux/ctype.h>

#include <linux/time.h>
#include <asm/unistd.h>
#include <linux/termios.h>
#include <linux/types.h>
*/

// See xbee/platform.h for function documentation.
int xbee_readline( char *buffer, int length)
{
  return -ENOSYS;
}
