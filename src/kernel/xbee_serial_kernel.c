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
  @file xbee_serial_kernel.c
  Serial Interface for XBee Module (Kernel Code)

  This file was created by Christian Stewart <christian@paral.in> based on
  information from:

  http://www.easysw.com/~mike/serial/serial.html

  and the xbee_serial_posix.c file.
*/
// NOTE: Documentation for these functions can be found in xbee/serial.h.

#include <linux/limits.h>
#include <linux/errno.h>
// #include <fs/fcntl.h>
#include <linux/termios.h>

#include "xbee/serial.h"

#define XBEE_SER_CHECK(ptr)	\
  {if (xbee_ser_invalid(ptr)) return -EINVAL; }


int xbee_ser_invalid( xbee_serial_t *serial)
{
  if (serial && serial->tty && serial->ldisc_ops)
    return 0;

#ifdef XBEE_SERIAL_VERBOSE
  if (serial == NULL)
    PRINT( "%s: serial=NULL\n", __FUNCTION__);
  else
    PRINT( "%s: serial=%p, serial->tty=%p, serial->ldisc_ops=%p (invalid)\n", __FUNCTION__,
        serial, serial->tty, serial->ldisc_ops);
#endif

  return 1;
}


const char *xbee_ser_portname( xbee_serial_t *serial)
{
  if (!serial)
    return "(invalid)";

  return serial->tty->name;
}


int xbee_ser_write( xbee_serial_t *serial, const void FAR *buffer,
    int length)
{
  int result;

  XBEE_SER_CHECK(serial);
  if (length < 0)
    return -EINVAL;

  if (!serial->ldisc_ops->write)
    result = serial->tty->driver->ops->write(serial->tty, buffer, length);
  else
    result = serial->ldisc_ops->write(serial->tty, NULL, buffer, length);

  if (result < 0)
  {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: error trying to write %d bytes, write() returned %d\n", __FUNCTION__, length, result);
#endif
    return -EIO;
  }

#ifdef XBEE_SERIAL_VERBOSE
  PRINT( "%s: wrote %d of %d bytes\n", __FUNCTION__, result, length);
#endif

  return result;
}


int xbee_ser_read( xbee_serial_t *serial, void FAR *buffer, int bufsize)
{
  int result;

  XBEE_SER_CHECK( serial);

  if (! buffer || bufsize < 0)
  {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: buffer=%p, bufsize=%d; return -EINVAL\n", __FUNCTION__,
        buffer, bufsize);
#endif
    return -EINVAL;
  }

  if (!serial->ldisc_ops->read)
  {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: ldisc_ops read NULL; return -ENOSYS\n", __FUNCTION__);
#endif
    return -ENOSYS;
  }

  result = serial->ldisc_ops->read(serial->tty, NULL, buffer, bufsize);
  if (result == 0)
    return 0;
  else if (result < 0) {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: error %d trying to read %d bytes\n", __FUNCTION__,
        result, bufsize);
#endif
    return result;
  }

#ifdef XBEE_SERIAL_VERBOSE
  PRINT( "%s: read %d bytes\n", __FUNCTION__, result);
#endif

  return result;
}


int xbee_ser_putchar( xbee_serial_t *serial, uint8_t ch)
{
  int retval;

  retval = xbee_ser_write( serial, &ch, 1);
  if (retval == 1)
    return 0;
  else if (retval == 0)
    return -ENOSPC;
  else
    return retval;
}


int xbee_ser_getchar( xbee_serial_t *serial)
{
  uint8_t ch = 0;
  int retval;

  retval = xbee_ser_read( serial, &ch, 1);
  if (retval != 1)
  {
    return retval ? retval : -ENODATA;
  }

  return ch;
}


int xbee_ser_tx_free( xbee_serial_t *serial)
{
  XBEE_SER_CHECK( serial);
  return INT_MAX;
}


int xbee_ser_tx_used( xbee_serial_t *serial)
{
  XBEE_SER_CHECK( serial);
  return 0;
}


int xbee_ser_tx_flush( xbee_serial_t *serial)
{
  XBEE_SER_CHECK( serial);
  // serial->tty->driver->ops->wait_until_sent();
  // return 0
  return -ENOSYS;
}


int xbee_ser_rx_free( xbee_serial_t *serial)
{
  XBEE_SER_CHECK( serial);
  return INT_MAX;
}


int xbee_ser_rx_used( xbee_serial_t *serial)
{
  int bytes;
  int err;

  XBEE_SER_CHECK( serial);

  if (!serial->ldisc_ops->ioctl) {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: ioctl %s failed, ioctl function not implemented\n", __FUNCTION__, "TIOCMGET");
#endif
    return -ENOSYS;
  }

  if ((err = serial->ldisc_ops->ioctl(serial->tty, NULL,
#ifdef FIONREAD
        FIONREAD,
#else
        TIOCINQ,							// for Cygwin
#endif
       (unsigned long)&bytes)) < 0)
  {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: ioctl %s failed (errno=%d)\n", __FUNCTION__,
        "TIOCMGET", err);
#endif
    return err;
  }
#ifdef XBEE_SERIAL_VERBOSE
  PRINT( "%s: ioctl %s returned (successfully) %d\n", __FUNCTION__,
        "TIOCMGET", bytes);
#endif
  return bytes;
}

int xbee_ser_rx_flush( xbee_serial_t *serial)
{
  XBEE_SER_CHECK( serial);

  if (!serial->ldisc_ops->flush_buffer)
    return -ENOSYS;

  serial->ldisc_ops->flush_buffer(serial->tty);
  return 0;
}


#define _BAUDCASE(b)		case b: baud = B ## b; break
int xbee_ser_baudrate( xbee_serial_t *serial, uint32_t baudrate)
{
  return -ENOSYS;
}


int xbee_ser_open( xbee_serial_t *serial, uint32_t baudrate)
{
  return -ENOSYS;
}


int xbee_ser_close( xbee_serial_t *serial)
{
  return -ENOSYS;
}


int xbee_ser_break( xbee_serial_t *serial, int enabled)
{
  int err;
  XBEE_SER_CHECK( serial);

  if (!serial->ldisc_ops->ioctl) {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: ioctl %s failed, ioctl function not implemented\n", __FUNCTION__, "TIOCMGET");
#endif
    return -ENOSYS;
  }

  if ((err = serial->ldisc_ops->ioctl(serial->tty, NULL, enabled ? TIOCSBRK : TIOCCBRK, 0)) < 0)
  {
#ifdef XBEE_SERIAL_VERBOSE
    PRINT( "%s: ioctl %s failed (errno=%d)\n", __FUNCTION__,
        "TIOCMGET", err);
#endif
    return err;
  }

  return 0;
}


int xbee_ser_flowcontrol( xbee_serial_t *serial, int enabled)
{
  XBEE_SER_CHECK( serial);
  return -ENOSYS;
}


int xbee_ser_set_rts( xbee_serial_t *serial, int asserted)
{
  XBEE_SER_CHECK( serial);
  return -ENOSYS;
}


int xbee_ser_get_cts( xbee_serial_t *serial)
{
  XBEE_SER_CHECK( serial);
  return -ENOSYS;
}

//@}
