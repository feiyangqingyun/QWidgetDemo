/*
 * Copyright © Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MODBUS_TCP_PRIVATE_H
#define MODBUS_TCP_PRIVATE_H

#define _MODBUS_TCP_HEADER_LENGTH     7
#define _MODBUS_TCP_PRESET_REQ_LENGTH 12
#define _MODBUS_TCP_PRESET_RSP_LENGTH 8

#define _MODBUS_TCP_CHECKSUM_LENGTH 0

/* In both structures, the transaction ID must be placed on first position
   to have a quick access not dependent of the TCP backend */
typedef struct _modbus_tcp {
    /* Extract from MODBUS Messaging on TCP/IP Implementation Guide V1.0b
       (page 23/46):
       The transaction identifier is used to associate the future response
       with the request. This identifier is unique on each TCP connection. */
    uint16_t t_id;
    /* TCP port */
    int port;
    /* IP address */
    char ip[16];
} modbus_tcp_t;

typedef struct _modbus_tcp_pi {
    /* Transaction ID */
    uint16_t t_id;
    /* TCP port */
    int port;
    /* Node */
    char *node;
    /* Service */
    char *service;
} modbus_tcp_pi_t;

#endif /* MODBUS_TCP_PRIVATE_H */
