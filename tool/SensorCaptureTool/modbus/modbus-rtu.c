/*
 * Copyright © Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include "modbus-private.h"
#include <assert.h>

#include "modbus-rtu-private.h"
#include "modbus-rtu.h"

#if HAVE_DECL_TIOCSRS485 || HAVE_DECL_TIOCM_RTS
#include <sys/ioctl.h>
#endif

#if HAVE_DECL_TIOCSRS485
#include <linux/serial.h>
#endif

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5,
    0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B,
    0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE,
    0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
    0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8,
    0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21,
    0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A,
    0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7,
    0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51,
    0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D,
    0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40};

/* Define the slave ID of the remote device to talk in master mode or set the
 * internal slave ID in slave mode */
static int _modbus_set_slave(modbus_t *ctx, int slave)
{
    int max_slave = (ctx->quirks & MODBUS_QUIRK_MAX_SLAVE) ? 255 : 247;

    /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */
    if (slave >= 0 && slave <= max_slave) {
        ctx->slave = slave;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

/* Builds a RTU request header */
static int _modbus_rtu_build_request_basis(
    modbus_t *ctx, int function, int addr, int nb, uint8_t *req)
{
    assert(ctx->slave != -1);
    req[0] = ctx->slave;
    req[1] = function;
    req[2] = addr >> 8;
    req[3] = addr & 0x00ff;
    req[4] = nb >> 8;
    req[5] = nb & 0x00ff;

    return _MODBUS_RTU_PRESET_REQ_LENGTH;
}

/* Builds a RTU response header */
static int _modbus_rtu_build_response_basis(sft_t *sft, uint8_t *rsp)
{
    /* In this case, the slave is certainly valid because a check is already
     * done in _modbus_rtu_listen */
    rsp[0] = sft->slave;
    rsp[1] = sft->function;

    return _MODBUS_RTU_PRESET_RSP_LENGTH;
}

static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i;        /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_lo ^ *buffer++; /* calculate the CRC  */
        crc_lo = crc_hi ^ table_crc_hi[i];
        crc_hi = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

static int _modbus_rtu_prepare_response_tid(const uint8_t *req, int *req_length)
{
    (*req_length) -= _MODBUS_RTU_CHECKSUM_LENGTH;
    /* No TID */
    return 0;
}

static int _modbus_rtu_send_msg_pre(uint8_t *req, int req_length)
{
    uint16_t crc = crc16(req, req_length);

    /* According to the MODBUS specs (p. 14), the low order byte of the CRC comes
     * first in the RTU message */
    req[req_length++] = crc & 0x00FF;
    req[req_length++] = crc >> 8;

    return req_length;
}

#if defined(_WIN32)

/* This simple implementation is sort of a substitute of the select() call,
 * working this way: the win32_ser_select() call tries to read some data from
 * the serial port, setting the timeout as the select() call would. Data read is
 * stored into the receive buffer, that is then consumed by the win32_ser_read()
 * call.  So win32_ser_select() does both the event waiting and the reading,
 * while win32_ser_read() only consumes the receive buffer.
 */

static void win32_ser_init(struct win32_ser *ws)
{
    /* Clear everything */
    memset(ws, 0x00, sizeof(struct win32_ser));

    /* Set file handle to invalid */
    ws->fd = INVALID_HANDLE_VALUE;
}

/* FIXME Try to remove length_to_read -> max_len argument, only used by win32 */
static int win32_ser_select(struct win32_ser *ws, int max_len, const struct timeval *tv)
{
    COMMTIMEOUTS comm_to;
    unsigned int msec = 0;

    /* Check if some data still in the buffer to be consumed */
    if (ws->n_bytes > 0) {
        return 1;
    }

    /* Setup timeouts like select() would do.
       FIXME Please someone on Windows can look at this?
       Does it possible to use WaitCommEvent?
       When tv is NULL, MAXDWORD isn't infinite!
     */
    if (tv == NULL) {
        msec = MAXDWORD;
    } else {
        msec = tv->tv_sec * 1000 + tv->tv_usec / 1000;
        if (msec < 1)
            msec = 1;
    }

    comm_to.ReadIntervalTimeout = msec;
    comm_to.ReadTotalTimeoutMultiplier = 0;
    comm_to.ReadTotalTimeoutConstant = msec;
    comm_to.WriteTotalTimeoutMultiplier = 0;
    comm_to.WriteTotalTimeoutConstant = 1000;
    SetCommTimeouts(ws->fd, &comm_to);

    /* Read some bytes */
    if ((max_len > PY_BUF_SIZE) || (max_len < 0)) {
        max_len = PY_BUF_SIZE;
    }

    if (ReadFile(ws->fd, &ws->buf, max_len, &ws->n_bytes, NULL)) {
        /* Check if some bytes available */
        if (ws->n_bytes > 0) {
            /* Some bytes read */
            return 1;
        } else {
            /* Just timed out */
            return 0;
        }
    } else {
        /* Some kind of error */
        return -1;
    }
}

static int win32_ser_read(struct win32_ser *ws, uint8_t *p_msg, unsigned int max_len)
{
    unsigned int n = ws->n_bytes;

    if (max_len < n) {
        n = max_len;
    }

    if (n > 0) {
        memcpy(p_msg, ws->buf, n);
    }

    ws->n_bytes -= n;

    return n;
}
#endif

#if HAVE_DECL_TIOCM_RTS
static void _modbus_rtu_ioctl_rts(modbus_t *ctx, int on)
{
    int fd = ctx->s;
    int flags;

    ioctl(fd, TIOCMGET, &flags);
    if (on) {
        flags |= TIOCM_RTS;
    } else {
        flags &= ~TIOCM_RTS;
    }
    ioctl(fd, TIOCMSET, &flags);
}
#endif

static ssize_t _modbus_rtu_send(modbus_t *ctx, const uint8_t *req, int req_length)
{
#if defined(_WIN32)
    modbus_rtu_t *ctx_rtu = ctx->backend_data;
    DWORD n_bytes = 0;
    return (WriteFile(ctx_rtu->w_ser.fd, req, req_length, &n_bytes, NULL))
               ? (ssize_t) n_bytes
               : -1;
#else
#if HAVE_DECL_TIOCM_RTS
    modbus_rtu_t *ctx_rtu = ctx->backend_data;
    if (ctx_rtu->rts != MODBUS_RTU_RTS_NONE) {
        ssize_t size;

        if (ctx->debug) {
            fprintf(stderr, "Sending request using RTS signal\n");
        }

        ctx_rtu->set_rts(ctx, ctx_rtu->rts == MODBUS_RTU_RTS_UP);
        usleep(ctx_rtu->rts_delay);

        size = write(ctx->s, req, req_length);

        usleep(ctx_rtu->onebyte_time * req_length + ctx_rtu->rts_delay);
        ctx_rtu->set_rts(ctx, ctx_rtu->rts != MODBUS_RTU_RTS_UP);

        return size;
    } else {
#endif
        return write(ctx->s, req, req_length);
#if HAVE_DECL_TIOCM_RTS
    }
#endif
#endif
}

static int _modbus_rtu_receive(modbus_t *ctx, uint8_t *req)
{
    int rc;
    modbus_rtu_t *ctx_rtu = ctx->backend_data;

    if (ctx_rtu->confirmation_to_ignore) {
        _modbus_receive_msg(ctx, req, MSG_CONFIRMATION);
        /* Ignore errors and reset the flag */
        ctx_rtu->confirmation_to_ignore = FALSE;
        rc = 0;
        if (ctx->debug) {
            printf("Confirmation to ignore\n");
        }
    } else {
        rc = _modbus_receive_msg(ctx, req, MSG_INDICATION);
        if (rc == 0) {
            /* The next expected message is a confirmation to ignore */
            ctx_rtu->confirmation_to_ignore = TRUE;
        }
    }
    return rc;
}

static ssize_t _modbus_rtu_recv(modbus_t *ctx, uint8_t *rsp, int rsp_length)
{
#if defined(_WIN32)
    return win32_ser_read(&((modbus_rtu_t *) ctx->backend_data)->w_ser, rsp, rsp_length);
#else
    return read(ctx->s, rsp, rsp_length);
#endif
}

static int _modbus_rtu_flush(modbus_t *);

static int _modbus_rtu_pre_check_confirmation(modbus_t *ctx,
                                              const uint8_t *req,
                                              const uint8_t *rsp,
                                              int rsp_length)
{
    /* Check responding slave is the slave we requested (except for broacast
     * request) */
    if (req[0] != rsp[0] && req[0] != MODBUS_BROADCAST_ADDRESS) {
        if (ctx->debug) {
            fprintf(stderr,
                    "The responding slave %d isn't the requested slave %d\n",
                    rsp[0],
                    req[0]);
        }
        errno = EMBBADSLAVE;
        return -1;
    } else {
        return 0;
    }
}

/* The check_crc16 function shall return 0 if the message is ignored and the
   message length if the CRC is valid. Otherwise it shall return -1 and set
   errno to EMBBADCRC. */
static int _modbus_rtu_check_integrity(modbus_t *ctx, uint8_t *msg, const int msg_length)
{
    uint16_t crc_calculated;
    uint16_t crc_received;
    int slave = msg[0];

    /* Filter on the Modbus unit identifier (slave) in RTU mode to avoid useless
     * CRC computing. */
    if (slave != ctx->slave && slave != MODBUS_BROADCAST_ADDRESS) {
        if (ctx->debug) {
            printf("Request for slave %d ignored (not %d)\n", slave, ctx->slave);
        }
        /* Following call to check_confirmation handles this error */
        return 0;
    }

    crc_calculated = crc16(msg, msg_length - 2);
    crc_received = (msg[msg_length - 1] << 8) | msg[msg_length - 2];

    /* Check CRC of msg */
    if (crc_calculated == crc_received) {
        return msg_length;
    } else {
        if (ctx->debug) {
            fprintf(stderr,
                    "ERROR CRC received 0x%0X != CRC calculated 0x%0X\n",
                    crc_received,
                    crc_calculated);
        }

        if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_PROTOCOL) {
            _modbus_rtu_flush(ctx);
        }
        errno = EMBBADCRC;
        return -1;
    }
}

/* Sets up a serial port for RTU communications */
#if defined(_WIN32)
static int _modbus_rtu_connect(modbus_t *ctx)
{
    DCB dcb;
    modbus_rtu_t *ctx_rtu = ctx->backend_data;

    if (ctx->debug) {
        printf("Opening %s at %d bauds (%c, %d, %d)\n",
               ctx_rtu->device,
               ctx_rtu->baud,
               ctx_rtu->parity,
               ctx_rtu->data_bit,
               ctx_rtu->stop_bit);
    }

    /* Some references here:
     * http://msdn.microsoft.com/en-us/library/aa450602.aspx
     */
    win32_ser_init(&ctx_rtu->w_ser);

    /* ctx_rtu->device should contain a string like "COMxx:" xx being a decimal
     * number */
    ctx_rtu->w_ser.fd = CreateFileA(
        ctx_rtu->device, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    /* Error checking */
    if (ctx_rtu->w_ser.fd == INVALID_HANDLE_VALUE) {
        if (ctx->debug) {
            fprintf(stderr,
                    "ERROR Can't open the device %s (LastError %d)\n",
                    ctx_rtu->device,
                    (int) GetLastError());
        }
        return -1;
    }

    /* Save params */
    ctx_rtu->old_dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(ctx_rtu->w_ser.fd, &ctx_rtu->old_dcb)) {
        if (ctx->debug) {
            fprintf(stderr,
                    "ERROR Error getting configuration (LastError %d)\n",
                    (int) GetLastError());
        }
        CloseHandle(ctx_rtu->w_ser.fd);
        ctx_rtu->w_ser.fd = INVALID_HANDLE_VALUE;
        return -1;
    }

    /* Build new configuration (starting from current settings) */
    dcb = ctx_rtu->old_dcb;

    /* Speed setting */
    dcb.BaudRate = ctx_rtu->baud;

    /* Data bits */
    switch (ctx_rtu->data_bit) {
    case 5:
        dcb.ByteSize = 5;
        break;
    case 6:
        dcb.ByteSize = 6;
        break;
    case 7:
        dcb.ByteSize = 7;
        break;
    case 8:
    default:
        dcb.ByteSize = 8;
        break;
    }

    /* Stop bits */
    if (ctx_rtu->stop_bit == 1)
        dcb.StopBits = ONESTOPBIT;
    else /* 2 */
        dcb.StopBits = TWOSTOPBITS;

    /* Parity */
    if (ctx_rtu->parity == 'N') {
        dcb.Parity = NOPARITY;
        dcb.fParity = FALSE;
    } else if (ctx_rtu->parity == 'E') {
        dcb.Parity = EVENPARITY;
        dcb.fParity = TRUE;
    } else {
        /* odd */
        dcb.Parity = ODDPARITY;
        dcb.fParity = TRUE;
    }

    /* Hardware handshaking left as default settings retrieved */

    /* No software handshaking */
    dcb.fTXContinueOnXoff = TRUE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;

    /* Binary mode (it's the only supported on Windows anyway) */
    dcb.fBinary = TRUE;

    /* Don't want errors to be blocking */
    dcb.fAbortOnError = FALSE;

    /* Setup port */
    if (!SetCommState(ctx_rtu->w_ser.fd, &dcb)) {
        if (ctx->debug) {
            fprintf(stderr,
                    "ERROR Error setting new configuration (LastError %d)\n",
                    (int) GetLastError());
        }
        CloseHandle(ctx_rtu->w_ser.fd);
        ctx_rtu->w_ser.fd = INVALID_HANDLE_VALUE;
        return -1;
    }

    return 0;
}
#else

static speed_t _get_termios_speed(int baud, int debug)
{
    speed_t speed;

    switch (baud) {
    case 110:
        speed = B110;
        break;
    case 300:
        speed = B300;
        break;
    case 600:
        speed = B600;
        break;
    case 1200:
        speed = B1200;
        break;
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
#ifdef B57600
    case 57600:
        speed = B57600;
        break;
#endif
#ifdef B115200
    case 115200:
        speed = B115200;
        break;
#endif
#ifdef B230400
    case 230400:
        speed = B230400;
        break;
#endif
#ifdef B460800
    case 460800:
        speed = B460800;
        break;
#endif
#ifdef B500000
    case 500000:
        speed = B500000;
        break;
#endif
#ifdef B576000
    case 576000:
        speed = B576000;
        break;
#endif
#ifdef B921600
    case 921600:
        speed = B921600;
        break;
#endif
#ifdef B1000000
    case 1000000:
        speed = B1000000;
        break;
#endif
#ifdef B1152000
    case 1152000:
        speed = B1152000;
        break;
#endif
#ifdef B1500000
    case 1500000:
        speed = B1500000;
        break;
#endif
#ifdef B2500000
    case 2500000:
        speed = B2500000;
        break;
#endif
#ifdef B3000000
    case 3000000:
        speed = B3000000;
        break;
#endif
#ifdef B3500000
    case 3500000:
        speed = B3500000;
        break;
#endif
#ifdef B4000000
    case 4000000:
        speed = B4000000;
        break;
#endif
    default:
        speed = B9600;
        if (debug) {
            fprintf(stderr, "WARNING Unknown baud rate %d (B9600 used)\n", baud);
        }
    }

    return speed;
}

/* POSIX */
static int _modbus_rtu_connect(modbus_t *ctx)
{
    struct termios tios;
    int flags;
    speed_t speed;
    modbus_rtu_t *ctx_rtu = ctx->backend_data;

    if (ctx->debug) {
        printf("Opening %s at %d bauds (%c, %d, %d)\n",
               ctx_rtu->device,
               ctx_rtu->baud,
               ctx_rtu->parity,
               ctx_rtu->data_bit,
               ctx_rtu->stop_bit);
    }

    /* The O_NOCTTY flag tells UNIX that this program doesn't want
       to be the "controlling terminal" for that port. If you
       don't specify this then any input (such as keyboard abort
       signals and so forth) will affect your process

       Timeouts are ignored in canonical input mode or when the
       NDELAY option is set on the file via open or fcntl */
    flags = O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL;
#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif

    ctx->s = open(ctx_rtu->device, flags);
    if (ctx->s < 0) {
        if (ctx->debug) {
            fprintf(stderr,
                    "ERROR Can't open the device %s (%s)\n",
                    ctx_rtu->device,
                    strerror(errno));
        }
        return -1;
    }

    /* Save */
    tcgetattr(ctx->s, &ctx_rtu->old_tios);

    memset(&tios, 0, sizeof(struct termios));

    /* C_ISPEED     Input baud (new interface)
       C_OSPEED     Output baud (new interface)
    */

    /* Set the baud rate */

    /*
    On MacOS, constants of baud rates are equal to the integer in argument but
    that's not the case under Linux so we have to find the corresponding
    constant. Until the code is upgraded to termios2, the list of possible
    values is limited (no 14400 for example).
    */
    if (9600 == B9600) {
        speed = ctx_rtu->baud;
    } else {
        speed = _get_termios_speed(ctx_rtu->baud, ctx->debug);
    }

    if ((cfsetispeed(&tios, speed) < 0) || (cfsetospeed(&tios, speed) < 0)) {
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }

    /* C_CFLAG      Control options
       CLOCAL       Local line - do not change "owner" of port
       CREAD        Enable receiver
    */
    tios.c_cflag |= (CREAD | CLOCAL);
    /* CSIZE, HUPCL, CRTSCTS (hardware flow control) */

    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
    */
    tios.c_cflag &= ~CSIZE;
    switch (ctx_rtu->data_bit) {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
    default:
        tios.c_cflag |= CS8;
        break;
    }

    /* Stop bit (1 or 2) */
    if (ctx_rtu->stop_bit == 1)
        tios.c_cflag &= ~CSTOPB;
    else /* 2 */
        tios.c_cflag |= CSTOPB;

    /* PARENB       Enable parity bit
       PARODD       Use odd parity instead of even */
    if (ctx_rtu->parity == 'N') {
        /* None */
        tios.c_cflag &= ~PARENB;
    } else if (ctx_rtu->parity == 'E') {
        /* Even */
        tios.c_cflag |= PARENB;
        tios.c_cflag &= ~PARODD;
    } else {
        /* Odd */
        tios.c_cflag |= PARENB;
        tios.c_cflag |= PARODD;
    }

    /* Read the man page of termios if you need more information. */

    /* This field isn't used on POSIX systems
       tios.c_line = 0;
    */

    /* C_LFLAG      Line options

       ISIG Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
       ICANON       Enable canonical input (else raw)
       XCASE        Map uppercase \lowercase (obsolete)
       ECHO Enable echoing of input characters
       ECHOE        Echo erase character as BS-SP-BS
       ECHOK        Echo NL after kill character
       ECHONL       Echo NL
       NOFLSH       Disable flushing of input buffers after
       interrupt or quit characters
       IEXTEN       Enable extended functions
       ECHOCTL      Echo control characters as ^char and delete as ~?
       ECHOPRT      Echo erased character as character erased
       ECHOKE       BS-SP-BS entire line on line kill
       FLUSHO       Output being flushed
       PENDIN       Retype pending input at next read or input char
       TOSTOP       Send SIGTTOU for background output

       Canonical input is line-oriented. Input characters are put
       into a buffer which can be edited interactively by the user
       until a CR (carriage return) or LF (line feed) character is
       received.

       Raw input is unprocessed. Input characters are passed
       through exactly as they are received, when they are
       received. Generally you'll deselect the ICANON, ECHO,
       ECHOE, and ISIG options when using raw input
    */

    /* Raw input */
    tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* C_IFLAG      Input options

       Constant     Description
       INPCK        Enable parity check
       IGNPAR       Ignore parity errors
       PARMRK       Mark parity errors
       ISTRIP       Strip parity bits
       IXON Enable software flow control (outgoing)
       IXOFF        Enable software flow control (incoming)
       IXANY        Allow any character to start flow again
       IGNBRK       Ignore break condition
       BRKINT       Send a SIGINT when a break condition is detected
       INLCR        Map NL to CR
       IGNCR        Ignore CR
       ICRNL        Map CR to NL
       IUCLC        Map uppercase to lowercase
       IMAXBEL      Echo BEL on input line too long
    */
    if (ctx_rtu->parity == 'N') {
        /* None */
        tios.c_iflag &= ~INPCK;
    } else {
        tios.c_iflag |= INPCK;
    }

    /* Software flow control is disabled */
    tios.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* C_OFLAG      Output options
       OPOST        Postprocess output (not set = raw output)
       ONLCR        Map NL to CR-NL

       ONCLR ant others needs OPOST to be enabled
    */

    /* Raw output */
    tios.c_oflag &= ~OPOST;

    /* C_CC         Control characters
       VMIN         Minimum number of characters to read
       VTIME        Time to wait for data (tenths of seconds)

       UNIX serial interface drivers provide the ability to
       specify character and packet timeouts. Two elements of the
       c_cc array are used for timeouts: VMIN and VTIME. Timeouts
       are ignored in canonical input mode or when the NDELAY
       option is set on the file via open or fcntl.

       VMIN specifies the minimum number of characters to read. If
       it is set to 0, then the VTIME value specifies the time to
       wait for every character read. Note that this does not mean
       that a read call for N bytes will wait for N characters to
       come in. Rather, the timeout will apply to the first
       character and the read call will return the number of
       characters immediately available (up to the number you
       request).

       If VMIN is non-zero, VTIME specifies the time to wait for
       the first character read. If a character is read within the
       time given, any read will block (wait) until all VMIN
       characters are read. That is, once the first character is
       read, the serial interface driver expects to receive an
       entire packet of characters (VMIN bytes total). If no
       character is read within the time allowed, then the call to
       read returns 0. This method allows you to tell the serial
       driver you need exactly N bytes and any read call will
       return 0 or N bytes. However, the timeout only applies to
       the first character read, so if for some reason the driver
       misses one character inside the N byte packet then the read
       call could block forever waiting for additional input
       characters.

       VTIME specifies the amount of time to wait for incoming
       characters in tenths of seconds. If VTIME is set to 0 (the
       default), reads will block (wait) indefinitely unless the
       NDELAY option is set on the port with open or fcntl.
    */
    /* Unused because we use open with the NDELAY option */
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 0;

    if (tcsetattr(ctx->s, TCSANOW, &tios) < 0) {
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }

    return 0;
}
#endif

// FIXME Temporary solution before rewriting Windows RTU backend
static unsigned int _modbus_rtu_is_connected(modbus_t *ctx)
{
#if defined(_WIN32)
    modbus_rtu_t *ctx_rtu = ctx->backend_data;

    /* Check if file handle is valid */
    return ctx_rtu->w_ser.fd != INVALID_HANDLE_VALUE;
#else
    return ctx->s >= 0;
#endif
}

int modbus_rtu_set_serial_mode(modbus_t *ctx, int mode)
{
    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCSRS485
        modbus_rtu_t *ctx_rtu = ctx->backend_data;
        struct serial_rs485 rs485conf;

        if (mode == MODBUS_RTU_RS485) {
            // Get
            if (ioctl(ctx->s, TIOCGRS485, &rs485conf) < 0) {
                return -1;
            }
            // Set
            rs485conf.flags |= SER_RS485_ENABLED;
            if (ioctl(ctx->s, TIOCSRS485, &rs485conf) < 0) {
                return -1;
            }

            ctx_rtu->serial_mode = MODBUS_RTU_RS485;
            return 0;
        } else if (mode == MODBUS_RTU_RS232) {
            /* Turn off RS485 mode only if required */
            if (ctx_rtu->serial_mode == MODBUS_RTU_RS485) {
                /* The ioctl call is avoided because it can fail on some RS232 ports */
                if (ioctl(ctx->s, TIOCGRS485, &rs485conf) < 0) {
                    return -1;
                }
                rs485conf.flags &= ~SER_RS485_ENABLED;
                if (ioctl(ctx->s, TIOCSRS485, &rs485conf) < 0) {
                    return -1;
                }
            }
            ctx_rtu->serial_mode = MODBUS_RTU_RS232;
            return 0;
        }
#else
        if (ctx->debug) {
            fprintf(stderr, "This function isn't supported on your platform\n");
        }
        errno = ENOTSUP;
        return -1;
#endif
    }

    /* Wrong backend and invalid mode specified */
    errno = EINVAL;
    return -1;
}

int modbus_rtu_get_serial_mode(modbus_t *ctx)
{
    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCSRS485
        modbus_rtu_t *ctx_rtu = ctx->backend_data;
        return ctx_rtu->serial_mode;
#else
        if (ctx->debug) {
            fprintf(stderr, "This function isn't supported on your platform\n");
        }
        errno = ENOTSUP;
        return -1;
#endif
    } else {
        errno = EINVAL;
        return -1;
    }
}

int modbus_rtu_get_rts(modbus_t *ctx)
{
    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCM_RTS
        modbus_rtu_t *ctx_rtu = ctx->backend_data;
        return ctx_rtu->rts;
#else
        if (ctx->debug) {
            fprintf(stderr, "This function isn't supported on your platform\n");
        }
        errno = ENOTSUP;
        return -1;
#endif
    } else {
        errno = EINVAL;
        return -1;
    }
}

int modbus_rtu_set_rts(modbus_t *ctx, int mode)
{
    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCM_RTS
        modbus_rtu_t *ctx_rtu = ctx->backend_data;

        if (mode == MODBUS_RTU_RTS_NONE || mode == MODBUS_RTU_RTS_UP ||
            mode == MODBUS_RTU_RTS_DOWN) {
            ctx_rtu->rts = mode;

            /* Set the RTS bit in order to not reserve the RS485 bus */
            ctx_rtu->set_rts(ctx, ctx_rtu->rts != MODBUS_RTU_RTS_UP);

            return 0;
        } else {
            errno = EINVAL;
            return -1;
        }
#else
        if (ctx->debug) {
            fprintf(stderr, "This function isn't supported on your platform\n");
        }
        errno = ENOTSUP;
        return -1;
#endif
    }
    /* Wrong backend or invalid mode specified */
    errno = EINVAL;
    return -1;
}

int modbus_rtu_set_custom_rts(modbus_t *ctx, void (*set_rts)(modbus_t *ctx, int on))
{
    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCM_RTS
        modbus_rtu_t *ctx_rtu = ctx->backend_data;
        ctx_rtu->set_rts = set_rts;
        return 0;
#else
        if (ctx->debug) {
            fprintf(stderr, "This function isn't supported on your platform\n");
        }
        errno = ENOTSUP;
        return -1;
#endif
    } else {
        errno = EINVAL;
        return -1;
    }
}

int modbus_rtu_get_rts_delay(modbus_t *ctx)
{
    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCM_RTS
        modbus_rtu_t *ctx_rtu;
        ctx_rtu = (modbus_rtu_t *) ctx->backend_data;
        return ctx_rtu->rts_delay;
#else
        if (ctx->debug) {
            fprintf(stderr, "This function isn't supported on your platform\n");
        }
        errno = ENOTSUP;
        return -1;
#endif
    } else {
        errno = EINVAL;
        return -1;
    }
}

int modbus_rtu_set_rts_delay(modbus_t *ctx, int us)
{
    if (ctx == NULL || us < 0) {
        errno = EINVAL;
        return -1;
    }

    if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCM_RTS
        modbus_rtu_t *ctx_rtu;
        ctx_rtu = (modbus_rtu_t *) ctx->backend_data;
        ctx_rtu->rts_delay = us;
        return 0;
#else
        if (ctx->debug) {
            fprintf(stderr, "This function isn't supported on your platform\n");
        }
        errno = ENOTSUP;
        return -1;
#endif
    } else {
        errno = EINVAL;
        return -1;
    }
}

static void _modbus_rtu_close(modbus_t *ctx)
{
    /* Restore line settings and close file descriptor in RTU mode */
    modbus_rtu_t *ctx_rtu = ctx->backend_data;

#if defined(_WIN32)
    /* Revert settings */
    if (!SetCommState(ctx_rtu->w_ser.fd, &ctx_rtu->old_dcb) && ctx->debug) {
        fprintf(stderr,
                "ERROR Couldn't revert to configuration (LastError %d)\n",
                (int) GetLastError());
    }

    if (!CloseHandle(ctx_rtu->w_ser.fd) && ctx->debug) {
        fprintf(stderr,
                "ERROR Error while closing handle (LastError %d)\n",
                (int) GetLastError());
    }
#else
    if (ctx->s >= 0) {
        tcsetattr(ctx->s, TCSANOW, &ctx_rtu->old_tios);
        close(ctx->s);
        ctx->s = -1;
    }
#endif
}

static int _modbus_rtu_flush(modbus_t *ctx)
{
#if defined(_WIN32)
    modbus_rtu_t *ctx_rtu = ctx->backend_data;
    ctx_rtu->w_ser.n_bytes = 0;
    return (PurgeComm(ctx_rtu->w_ser.fd, PURGE_RXCLEAR) == FALSE);
#else
    return tcflush(ctx->s, TCIOFLUSH);
#endif
}

static int
_modbus_rtu_select(modbus_t *ctx, fd_set *rset, struct timeval *tv, int length_to_read)
{
    int s_rc;
#if defined(_WIN32)
    s_rc = win32_ser_select(
        &((modbus_rtu_t *) ctx->backend_data)->w_ser, length_to_read, tv);
    if (s_rc == 0) {
        errno = ETIMEDOUT;
        return -1;
    }

    if (s_rc < 0) {
        return -1;
    }
#else
    while ((s_rc = select(ctx->s + 1, rset, NULL, NULL, tv)) == -1) {
        if (errno == EINTR) {
            if (ctx->debug) {
                fprintf(stderr, "A non blocked signal was caught\n");
            }
            /* Necessary after an error */
            FD_ZERO(rset);
            FD_SET(ctx->s, rset);
        } else {
            return -1;
        }
    }

    if (s_rc == 0) {
        /* Timeout */
        errno = ETIMEDOUT;
        return -1;
    }
#endif

    return s_rc;
}

static void _modbus_rtu_free(modbus_t *ctx)
{
    if (ctx->backend_data) {
        free(((modbus_rtu_t *) ctx->backend_data)->device);
        free(ctx->backend_data);
    }

    free(ctx);
}

// clang-format off
const modbus_backend_t _modbus_rtu_backend = {
    _MODBUS_BACKEND_TYPE_RTU,
    _MODBUS_RTU_HEADER_LENGTH,
    _MODBUS_RTU_CHECKSUM_LENGTH,
    MODBUS_RTU_MAX_ADU_LENGTH,
    _modbus_set_slave,
    _modbus_rtu_build_request_basis,
    _modbus_rtu_build_response_basis,
    _modbus_rtu_prepare_response_tid,
    _modbus_rtu_send_msg_pre,
    _modbus_rtu_send,
    _modbus_rtu_receive,
    _modbus_rtu_recv,
    _modbus_rtu_check_integrity,
    _modbus_rtu_pre_check_confirmation,
    _modbus_rtu_connect,
    _modbus_rtu_is_connected,
    _modbus_rtu_close,
    _modbus_rtu_flush,
    _modbus_rtu_select,
    _modbus_rtu_free
};

// clang-format on

modbus_t *
modbus_new_rtu(const char *device, int baud, char parity, int data_bit, int stop_bit)
{
    modbus_t *ctx;
    modbus_rtu_t *ctx_rtu;

    /* Check device argument */
    if (device == NULL || *device == 0) {
        fprintf(stderr, "The device string is empty\n");
        errno = EINVAL;
        return NULL;
    }

    /* Check baud argument */
    if (baud == 0) {
        fprintf(stderr, "The baud rate value must not be zero\n");
        errno = EINVAL;
        return NULL;
    }

    ctx = (modbus_t *) malloc(sizeof(modbus_t));
    if (ctx == NULL) {
        return NULL;
    }

    _modbus_init_common(ctx);
    ctx->backend = &_modbus_rtu_backend;
    ctx->backend_data = (modbus_rtu_t *) malloc(sizeof(modbus_rtu_t));
    if (ctx->backend_data == NULL) {
        modbus_free(ctx);
        errno = ENOMEM;
        return NULL;
    }
    ctx_rtu = (modbus_rtu_t *) ctx->backend_data;

    /* Device name and \0 */
    ctx_rtu->device = (char *) malloc((strlen(device) + 1) * sizeof(char));
    if (ctx_rtu->device == NULL) {
        modbus_free(ctx);
        errno = ENOMEM;
        return NULL;
    }

#if defined(_WIN32)
    strcpy_s(ctx_rtu->device, strlen(device) + 1, device);
#else
    strcpy(ctx_rtu->device, device);
#endif

    ctx_rtu->baud = baud;
    if (parity == 'N' || parity == 'E' || parity == 'O') {
        ctx_rtu->parity = parity;
    } else {
        modbus_free(ctx);
        errno = EINVAL;
        return NULL;
    }
    ctx_rtu->data_bit = data_bit;
    ctx_rtu->stop_bit = stop_bit;

#if HAVE_DECL_TIOCSRS485
    /* The RS232 mode has been set by default */
    ctx_rtu->serial_mode = MODBUS_RTU_RS232;
#endif

#if HAVE_DECL_TIOCM_RTS
    /* The RTS use has been set by default */
    ctx_rtu->rts = MODBUS_RTU_RTS_NONE;

    /* Calculate estimated time in micro second to send one byte */
    ctx_rtu->onebyte_time =
        1000000 * (1 + data_bit + (parity == 'N' ? 0 : 1) + stop_bit) / baud;

    /* The internal function is used by default to set RTS */
    ctx_rtu->set_rts = _modbus_rtu_ioctl_rts;

    /* The delay before and after transmission when toggling the RTS pin */
    ctx_rtu->rts_delay = ctx_rtu->onebyte_time;
#endif

    ctx_rtu->confirmation_to_ignore = FALSE;

    return ctx;
}
