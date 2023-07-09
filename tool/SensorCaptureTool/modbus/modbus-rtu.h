/*
 * Copyright © Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include "modbus.h"

MODBUS_BEGIN_DECLS

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
#define MODBUS_RTU_MAX_ADU_LENGTH 256

MODBUS_API modbus_t *
modbus_new_rtu(const char *device, int baud, char parity, int data_bit, int stop_bit);

#define MODBUS_RTU_RS232 0
#define MODBUS_RTU_RS485 1

MODBUS_API int modbus_rtu_set_serial_mode(modbus_t *ctx, int mode);
MODBUS_API int modbus_rtu_get_serial_mode(modbus_t *ctx);

#define MODBUS_RTU_RTS_NONE 0
#define MODBUS_RTU_RTS_UP   1
#define MODBUS_RTU_RTS_DOWN 2

MODBUS_API int modbus_rtu_set_rts(modbus_t *ctx, int mode);
MODBUS_API int modbus_rtu_get_rts(modbus_t *ctx);

MODBUS_API int modbus_rtu_set_custom_rts(modbus_t *ctx,
                                         void (*set_rts)(modbus_t *ctx, int on));

MODBUS_API int modbus_rtu_set_rts_delay(modbus_t *ctx, int us);
MODBUS_API int modbus_rtu_get_rts_delay(modbus_t *ctx);

MODBUS_END_DECLS

#endif /* MODBUS_RTU_H */
