/*
 * Copyright © Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

// clang-format off
#if defined(_WIN32)
# define OS_WIN32
/* ws2_32.dll has getaddrinfo and freeaddrinfo on Windows XP and later.
 * minwg32 headers check WINVER before allowing the use of these */
# ifndef WINVER
#   define WINVER 0x0501
# endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <signal.h>
#include <sys/types.h>

#define _WIN32_WINNT 0x0600


#if defined(_WIN32)
/* Already set in modbus-tcp.h but it seems order matters in VS2005 */
# include <winsock2.h>
# include <ws2tcpip.h>
# define SHUT_RDWR 2
# define close closesocket
# define strdup _strdup
#else
# include <sys/socket.h>
# include <sys/ioctl.h>

#if defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ < 5)
# define OS_BSD
# include <netinet/in_systm.h>
#endif

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL 0
#endif

#if defined(_AIX) && !defined(MSG_DONTWAIT)
#define MSG_DONTWAIT MSG_NONBLOCK
#endif
// clang-format on

#include "modbus-private.h"

#include "modbus-tcp-private.h"
#include "modbus-tcp.h"

#ifdef OS_WIN32
static int _modbus_tcp_init_win32(void)
{
    /* Initialise Windows Socket API */
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr,
                "WSAStartup() returned error code %d\n",
                (unsigned int) GetLastError());
        errno = EIO;
        return -1;
    }
    return 0;
}
#endif

static int _modbus_set_slave(modbus_t *ctx, int slave)
{
    int max_slave = (ctx->quirks & MODBUS_QUIRK_MAX_SLAVE) ? 255 : 247;

    /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */
    if (slave >= 0 && slave <= max_slave) {
        ctx->slave = slave;
    } else if (slave == MODBUS_TCP_SLAVE) {
        /* The special value MODBUS_TCP_SLAVE (0xFF) can be used in TCP mode to
         * restore the default value. */
        ctx->slave = slave;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

/* Builds a TCP request header */
static int _modbus_tcp_build_request_basis(
    modbus_t *ctx, int function, int addr, int nb, uint8_t *req)
{
    modbus_tcp_t *ctx_tcp = ctx->backend_data;

    /* Increase transaction ID */
    if (ctx_tcp->t_id < UINT16_MAX)
        ctx_tcp->t_id++;
    else
        ctx_tcp->t_id = 0;
    req[0] = ctx_tcp->t_id >> 8;
    req[1] = ctx_tcp->t_id & 0x00ff;

    /* Protocol Modbus */
    req[2] = 0;
    req[3] = 0;

    /* Length will be defined later by set_req_length_tcp at offsets 4
       and 5 */

    req[6] = ctx->slave;
    req[7] = function;
    req[8] = addr >> 8;
    req[9] = addr & 0x00ff;
    req[10] = nb >> 8;
    req[11] = nb & 0x00ff;

    return _MODBUS_TCP_PRESET_REQ_LENGTH;
}

/* Builds a TCP response header */
static int _modbus_tcp_build_response_basis(sft_t *sft, uint8_t *rsp)
{
    /* Extract from MODBUS Messaging on TCP/IP Implementation
       Guide V1.0b (page 23/46):
       The transaction identifier is used to associate the future
       response with the request. */
    rsp[0] = sft->t_id >> 8;
    rsp[1] = sft->t_id & 0x00ff;

    /* Protocol Modbus */
    rsp[2] = 0;
    rsp[3] = 0;

    /* Length will be set later by send_msg (4 and 5) */

    /* The slave ID is copied from the indication */
    rsp[6] = sft->slave;
    rsp[7] = sft->function;

    return _MODBUS_TCP_PRESET_RSP_LENGTH;
}

static int _modbus_tcp_prepare_response_tid(const uint8_t *req, int *req_length)
{
    return (req[0] << 8) + req[1];
}

static int _modbus_tcp_send_msg_pre(uint8_t *req, int req_length)
{
    /* Subtract the header length to the message length */
    int mbap_length = req_length - 6;

    req[4] = mbap_length >> 8;
    req[5] = mbap_length & 0x00FF;

    return req_length;
}

static ssize_t _modbus_tcp_send(modbus_t *ctx, const uint8_t *req, int req_length)
{
    /* MSG_NOSIGNAL
       Requests not to send SIGPIPE on errors on stream oriented
       sockets when the other end breaks the connection.  The EPIPE
       error is still returned. */
    return send(ctx->s, (const char *) req, req_length, MSG_NOSIGNAL);
}

static int _modbus_tcp_receive(modbus_t *ctx, uint8_t *req)
{
    return _modbus_receive_msg(ctx, req, MSG_INDICATION);
}

static ssize_t _modbus_tcp_recv(modbus_t *ctx, uint8_t *rsp, int rsp_length)
{
    return recv(ctx->s, (char *) rsp, rsp_length, 0);
}

static int _modbus_tcp_check_integrity(modbus_t *ctx, uint8_t *msg, const int msg_length)
{
    return msg_length;
}

static int _modbus_tcp_pre_check_confirmation(modbus_t *ctx,
                                              const uint8_t *req,
                                              const uint8_t *rsp,
                                              int rsp_length)
{
    unsigned int protocol_id;
    /* Check transaction ID */
    if (req[0] != rsp[0] || req[1] != rsp[1]) {
        if (ctx->debug) {
            fprintf(stderr,
                    "Invalid transaction ID received 0x%X (not 0x%X)\n",
                    (rsp[0] << 8) + rsp[1],
                    (req[0] << 8) + req[1]);
        }
        errno = EMBBADDATA;
        return -1;
    }

    /* Check protocol ID */
    protocol_id = (rsp[2] << 8) + rsp[3];
    if (protocol_id != 0x0) {
        if (ctx->debug) {
            fprintf(stderr, "Invalid protocol ID received 0x%X (not 0x0)\n", protocol_id);
        }
        errno = EMBBADDATA;
        return -1;
    }

    return 0;
}

static int _modbus_tcp_set_ipv4_options(int s)
{
    int rc;
    int option;

    /* Set the TCP no delay flag */
    /* SOL_TCP = IPPROTO_TCP */
    option = 1;
    rc = setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const void *) &option, sizeof(int));
    if (rc == -1) {
        return -1;
    }

    /* If the OS does not offer SOCK_NONBLOCK, fall back to setting FIONBIO to
     * make sockets non-blocking */
    /* Do not care about the return value, this is optional */
#if !defined(SOCK_NONBLOCK) && defined(FIONBIO)
#ifdef OS_WIN32
    {
        /* Setting FIONBIO expects an unsigned long according to MSDN */
        u_long loption = 1;
        ioctlsocket(s, FIONBIO, &loption);
    }
#else
    option = 1;
    ioctl(s, FIONBIO, &option);
#endif
#endif

#ifndef OS_WIN32
    /**
     * Cygwin defines IPTOS_LOWDELAY but can't handle that flag so it's
     * necessary to workaround that problem.
     **/
    /* Set the IP low delay option */
    option = IPTOS_LOWDELAY;
    rc = setsockopt(s, IPPROTO_IP, IP_TOS, (const void *) &option, sizeof(int));
    if (rc == -1) {
        return -1;
    }
#endif

    return 0;
}

static int _connect(int sockfd,
                    const struct sockaddr *addr,
                    socklen_t addrlen,
                    const struct timeval *ro_tv)
{
    int rc = connect(sockfd, addr, addrlen);

#ifdef OS_WIN32
    int wsaError = 0;
    if (rc == -1) {
        wsaError = WSAGetLastError();
    }

    if (wsaError == WSAEWOULDBLOCK || wsaError == WSAEINPROGRESS) {
#else
    if (rc == -1 && errno == EINPROGRESS) {
#endif
        fd_set wset;
        int optval;
        socklen_t optlen = sizeof(optval);
        struct timeval tv = *ro_tv;

        /* Wait to be available in writing */
        FD_ZERO(&wset);
        FD_SET(sockfd, &wset);
        rc = select(sockfd + 1, NULL, &wset, NULL, &tv);
        if (rc <= 0) {
            /* Timeout or fail */
            return -1;
        }

        /* The connection is established if SO_ERROR and optval are set to 0 */
        rc = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *) &optval, &optlen);
        if (rc == 0 && optval == 0) {
            return 0;
        } else {
            errno = ECONNREFUSED;
            return -1;
        }
    }
    return rc;
}

/* Establishes a modbus TCP connection with a Modbus server. */
static int _modbus_tcp_connect(modbus_t *ctx)
{
    int rc;
    /* Specialized version of sockaddr for Internet socket address (same size) */
    struct sockaddr_in addr;
    modbus_tcp_t *ctx_tcp = ctx->backend_data;
    int flags = SOCK_STREAM;

#ifdef OS_WIN32
    if (_modbus_tcp_init_win32() == -1) {
        return -1;
    }
#endif

#ifdef SOCK_CLOEXEC
    flags |= SOCK_CLOEXEC;
#endif

#ifdef SOCK_NONBLOCK
    flags |= SOCK_NONBLOCK;
#endif

    ctx->s = socket(PF_INET, flags, 0);
    if (ctx->s < 0) {
        return -1;
    }

    rc = _modbus_tcp_set_ipv4_options(ctx->s);
    if (rc == -1) {
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }

    if (ctx->debug) {
        printf("Connecting to %s:%d\n", ctx_tcp->ip, ctx_tcp->port);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ctx_tcp->port);
    rc = inet_pton(addr.sin_family, ctx_tcp->ip, &(addr.sin_addr));
    if (rc <= 0) {
        if (ctx->debug) {
            fprintf(stderr, "Invalid IP address: %s\n", ctx_tcp->ip);
        }
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }

    rc =
        _connect(ctx->s, (struct sockaddr *) &addr, sizeof(addr), &ctx->response_timeout);
    if (rc == -1) {
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }

    return 0;
}

/* Establishes a modbus TCP PI connection with a Modbus server. */
static int _modbus_tcp_pi_connect(modbus_t *ctx)
{
    int rc;
    struct addrinfo *ai_list;
    struct addrinfo *ai_ptr;
    struct addrinfo ai_hints;
    modbus_tcp_pi_t *ctx_tcp_pi = ctx->backend_data;

#ifdef OS_WIN32
    if (_modbus_tcp_init_win32() == -1) {
        return -1;
    }
#endif

    memset(&ai_hints, 0, sizeof(ai_hints));
#ifdef AI_ADDRCONFIG
    ai_hints.ai_flags |= AI_ADDRCONFIG;
#endif
    ai_hints.ai_family = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_addr = NULL;
    ai_hints.ai_canonname = NULL;
    ai_hints.ai_next = NULL;

    ai_list = NULL;
    rc = getaddrinfo(ctx_tcp_pi->node, ctx_tcp_pi->service, &ai_hints, &ai_list);
    if (rc != 0) {
        if (ctx->debug) {
            fprintf(stderr, "Error returned by getaddrinfo: %s\n", gai_strerror(rc));
        }
        errno = ECONNREFUSED;
        return -1;
    }

    for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next) {
        int flags = ai_ptr->ai_socktype;
        int s;

#ifdef SOCK_CLOEXEC
        flags |= SOCK_CLOEXEC;
#endif

#ifdef SOCK_NONBLOCK
        flags |= SOCK_NONBLOCK;
#endif

        s = socket(ai_ptr->ai_family, flags, ai_ptr->ai_protocol);
        if (s < 0)
            continue;

        if (ai_ptr->ai_family == AF_INET)
            _modbus_tcp_set_ipv4_options(s);

        if (ctx->debug) {
            printf("Connecting to [%s]:%s\n", ctx_tcp_pi->node, ctx_tcp_pi->service);
        }

        rc = _connect(s, ai_ptr->ai_addr, ai_ptr->ai_addrlen, &ctx->response_timeout);
        if (rc == -1) {
            close(s);
            continue;
        }

        ctx->s = s;
        break;
    }

    freeaddrinfo(ai_list);

    if (ctx->s < 0) {
        return -1;
    }

    return 0;
}

static unsigned int _modbus_tcp_is_connected(modbus_t *ctx)
{
    return ctx->s >= 0;
}

/* Closes the network connection and socket in TCP mode */
static void _modbus_tcp_close(modbus_t *ctx)
{
    if (ctx->s >= 0) {
        shutdown(ctx->s, SHUT_RDWR);
        close(ctx->s);
        ctx->s = -1;
    }
}

static int _modbus_tcp_flush(modbus_t *ctx)
{
    int rc;
    int rc_sum = 0;

    do {
        /* Extract the garbage from the socket */
        char devnull[MODBUS_TCP_MAX_ADU_LENGTH];
#ifndef OS_WIN32
        rc = recv(ctx->s, devnull, MODBUS_TCP_MAX_ADU_LENGTH, MSG_DONTWAIT);
#else
        /* On Win32, it's a bit more complicated to not wait */
        fd_set rset;
        struct timeval tv;

        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&rset);
        FD_SET(ctx->s, &rset);
        rc = select(ctx->s + 1, &rset, NULL, NULL, &tv);
        if (rc == -1) {
            return -1;
        }

        if (rc == 1) {
            /* There is data to flush */
            rc = recv(ctx->s, devnull, MODBUS_TCP_MAX_ADU_LENGTH, 0);
        }
#endif
        if (rc > 0) {
            rc_sum += rc;
        }
    } while (rc == MODBUS_TCP_MAX_ADU_LENGTH);

    return rc_sum;
}

/* Listens for any request from one or many modbus masters in TCP */
int modbus_tcp_listen(modbus_t *ctx, int nb_connection)
{
    int new_s;
    int enable;
    int flags;
    struct sockaddr_in addr;
    modbus_tcp_t *ctx_tcp;
    int rc;

    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    ctx_tcp = ctx->backend_data;

#ifdef OS_WIN32
    if (_modbus_tcp_init_win32() == -1) {
        return -1;
    }
#endif

    flags = SOCK_STREAM;

#ifdef SOCK_CLOEXEC
    flags |= SOCK_CLOEXEC;
#endif

    new_s = socket(PF_INET, flags, IPPROTO_TCP);
    if (new_s == -1) {
        return -1;
    }

    enable = 1;
    if (setsockopt(new_s, SOL_SOCKET, SO_REUSEADDR, (char *) &enable, sizeof(enable)) ==
        -1) {
        close(new_s);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    /* If the modbus port is < to 1024, we need the setuid root. */
    addr.sin_port = htons(ctx_tcp->port);
    if (ctx_tcp->ip[0] == '0') {
        /* Listen any addresses */
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        /* Listen only specified IP address */
        rc = inet_pton(addr.sin_family, ctx_tcp->ip, &(addr.sin_addr));
        if (rc <= 0) {
            if (ctx->debug) {
                fprintf(stderr, "Invalid IP address: %s\n", ctx_tcp->ip);
            }
            close(new_s);
            return -1;
        }
    }

    if (bind(new_s, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        close(new_s);
        return -1;
    }

    if (listen(new_s, nb_connection) == -1) {
        close(new_s);
        return -1;
    }

    return new_s;
}

int modbus_tcp_pi_listen(modbus_t *ctx, int nb_connection)
{
    int rc;
    struct addrinfo *ai_list;
    struct addrinfo *ai_ptr;
    struct addrinfo ai_hints;
    const char *node;
    const char *service;
    int new_s;
    modbus_tcp_pi_t *ctx_tcp_pi;

    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    ctx_tcp_pi = ctx->backend_data;

#ifdef OS_WIN32
    if (_modbus_tcp_init_win32() == -1) {
        return -1;
    }
#endif

    if (ctx_tcp_pi->node[0] == 0) {
        node = NULL; /* == any */
    } else {
        node = ctx_tcp_pi->node;
    }

    if (ctx_tcp_pi->service[0] == 0) {
        service = "502";
    } else {
        service = ctx_tcp_pi->service;
    }

    memset(&ai_hints, 0, sizeof(ai_hints));
    /* If node is not NULL, than the AI_PASSIVE flag is ignored. */
    ai_hints.ai_flags |= AI_PASSIVE;
#ifdef AI_ADDRCONFIG
    ai_hints.ai_flags |= AI_ADDRCONFIG;
#endif
    ai_hints.ai_family = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_addr = NULL;
    ai_hints.ai_canonname = NULL;
    ai_hints.ai_next = NULL;

    ai_list = NULL;
    rc = getaddrinfo(node, service, &ai_hints, &ai_list);
    if (rc != 0) {
        if (ctx->debug) {
            fprintf(stderr, "Error returned by getaddrinfo: %s\n", gai_strerror(rc));
        }
        errno = ECONNREFUSED;
        return -1;
    }

    new_s = -1;
    for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next) {
        int flags = ai_ptr->ai_socktype;
        int s;

#ifdef SOCK_CLOEXEC
        flags |= SOCK_CLOEXEC;
#endif

        s = socket(ai_ptr->ai_family, flags, ai_ptr->ai_protocol);
        if (s < 0) {
            if (ctx->debug) {
                perror("socket");
            }
            continue;
        } else {
            int enable = 1;
            rc =
                setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *) &enable, sizeof(enable));
            if (rc != 0) {
                close(s);
                if (ctx->debug) {
                    perror("setsockopt");
                }
                continue;
            }
        }

        rc = bind(s, ai_ptr->ai_addr, ai_ptr->ai_addrlen);
        if (rc != 0) {
            close(s);
            if (ctx->debug) {
                perror("bind");
            }
            continue;
        }

        rc = listen(s, nb_connection);
        if (rc != 0) {
            close(s);
            if (ctx->debug) {
                perror("listen");
            }
            continue;
        }

        new_s = s;
        break;
    }
    freeaddrinfo(ai_list);

    if (new_s < 0) {
        return -1;
    }

    return new_s;
}

int modbus_tcp_accept(modbus_t *ctx, int *s)
{
    struct sockaddr_in addr;
    socklen_t addrlen;

    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    addrlen = sizeof(addr);
#ifdef HAVE_ACCEPT4
    /* Inherit socket flags and use accept4 call */
    ctx->s = accept4(*s, (struct sockaddr *) &addr, &addrlen, SOCK_CLOEXEC);
#else
    ctx->s = accept(*s, (struct sockaddr *) &addr, &addrlen);
#endif

    if (ctx->s < 0) {
        return -1;
    }

    if (ctx->debug) {
        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &(addr.sin_addr), buf, INET_ADDRSTRLEN) == NULL) {
            fprintf(stderr, "Client connection accepted from unparsable IP.\n");
        } else {
            printf("Client connection accepted from %s.\n", buf);
        }
    }

    return ctx->s;
}

int modbus_tcp_pi_accept(modbus_t *ctx, int *s)
{
    struct sockaddr_in6 addr;
    socklen_t addrlen;

    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    addrlen = sizeof(addr);
#ifdef HAVE_ACCEPT4
    /* Inherit socket flags and use accept4 call */
    ctx->s = accept4(*s, (struct sockaddr *) &addr, &addrlen, SOCK_CLOEXEC);
#else
    ctx->s = accept(*s, (struct sockaddr *) &addr, &addrlen);
#endif

    if (ctx->s < 0) {
        return -1;
    }

    if (ctx->debug) {
        char buf[INET6_ADDRSTRLEN];
        if (inet_ntop(AF_INET6, &(addr.sin6_addr), buf, INET6_ADDRSTRLEN) == NULL) {
            fprintf(stderr, "Client connection accepted from unparsable IP.\n");
        } else {
            printf("Client connection accepted from %s.\n", buf);
        }
    }

    return ctx->s;
}

static int
_modbus_tcp_select(modbus_t *ctx, fd_set *rset, struct timeval *tv, int length_to_read)
{
    int s_rc;
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
        errno = ETIMEDOUT;
        return -1;
    }

    return s_rc;
}

static void _modbus_tcp_free(modbus_t *ctx)
{
    if (ctx->backend_data) {
        free(ctx->backend_data);
    }
    free(ctx);
}

static void _modbus_tcp_pi_free(modbus_t *ctx)
{
    if (ctx->backend_data) {
        modbus_tcp_pi_t *ctx_tcp_pi = ctx->backend_data;
        free(ctx_tcp_pi->node);
        free(ctx_tcp_pi->service);
        free(ctx->backend_data);
    }

    free(ctx);
}

// clang-format off
const modbus_backend_t _modbus_tcp_backend = {
    _MODBUS_BACKEND_TYPE_TCP,
    _MODBUS_TCP_HEADER_LENGTH,
    _MODBUS_TCP_CHECKSUM_LENGTH,
    MODBUS_TCP_MAX_ADU_LENGTH,
    _modbus_set_slave,
    _modbus_tcp_build_request_basis,
    _modbus_tcp_build_response_basis,
    _modbus_tcp_prepare_response_tid,
    _modbus_tcp_send_msg_pre,
    _modbus_tcp_send,
    _modbus_tcp_receive,
    _modbus_tcp_recv,
    _modbus_tcp_check_integrity,
    _modbus_tcp_pre_check_confirmation,
    _modbus_tcp_connect,
    _modbus_tcp_is_connected,
    _modbus_tcp_close,
    _modbus_tcp_flush,
    _modbus_tcp_select,
    _modbus_tcp_free
};

const modbus_backend_t _modbus_tcp_pi_backend = {
    _MODBUS_BACKEND_TYPE_TCP,
    _MODBUS_TCP_HEADER_LENGTH,
    _MODBUS_TCP_CHECKSUM_LENGTH,
    MODBUS_TCP_MAX_ADU_LENGTH,
    _modbus_set_slave,
    _modbus_tcp_build_request_basis,
    _modbus_tcp_build_response_basis,
    _modbus_tcp_prepare_response_tid,
    _modbus_tcp_send_msg_pre,
    _modbus_tcp_send,
    _modbus_tcp_receive,
    _modbus_tcp_recv,
    _modbus_tcp_check_integrity,
    _modbus_tcp_pre_check_confirmation,
    _modbus_tcp_pi_connect,
    _modbus_tcp_is_connected,
    _modbus_tcp_close,
    _modbus_tcp_flush,
    _modbus_tcp_select,
    _modbus_tcp_pi_free
};

// clang-format on

modbus_t *modbus_new_tcp(const char *ip, int port)
{
    modbus_t *ctx;
    modbus_tcp_t *ctx_tcp;
    size_t dest_size;
    size_t ret_size;

#if defined(OS_BSD)
    /* MSG_NOSIGNAL is unsupported on *BSD so we install an ignore
       handler for SIGPIPE. */
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) < 0) {
        /* The debug flag can't be set here... */
        fprintf(stderr, "Could not install SIGPIPE handler.\n");
        return NULL;
    }
#endif

    ctx = (modbus_t *) malloc(sizeof(modbus_t));
    if (ctx == NULL) {
        return NULL;
    }
    _modbus_init_common(ctx);

    /* Could be changed after to reach a remote serial Modbus device */
    ctx->slave = MODBUS_TCP_SLAVE;

    ctx->backend = &_modbus_tcp_backend;

    ctx->backend_data = (modbus_tcp_t *) malloc(sizeof(modbus_tcp_t));
    if (ctx->backend_data == NULL) {
        modbus_free(ctx);
        errno = ENOMEM;
        return NULL;
    }
    ctx_tcp = (modbus_tcp_t *) ctx->backend_data;

    if (ip != NULL) {
        dest_size = sizeof(char) * 16;
        ret_size = strlcpy(ctx_tcp->ip, ip, dest_size);
        if (ret_size == 0) {
            fprintf(stderr, "The IP string is empty\n");
            modbus_free(ctx);
            errno = EINVAL;
            return NULL;
        }

        if (ret_size >= dest_size) {
            fprintf(stderr, "The IP string has been truncated\n");
            modbus_free(ctx);
            errno = EINVAL;
            return NULL;
        }
    } else {
        ctx_tcp->ip[0] = '0';
    }
    ctx_tcp->port = port;
    ctx_tcp->t_id = 0;

    return ctx;
}

modbus_t *modbus_new_tcp_pi(const char *node, const char *service)
{
    modbus_t *ctx;
    modbus_tcp_pi_t *ctx_tcp_pi;

    ctx = (modbus_t *) malloc(sizeof(modbus_t));
    if (ctx == NULL) {
        return NULL;
    }
    _modbus_init_common(ctx);

    /* Could be changed after to reach a remote serial Modbus device */
    ctx->slave = MODBUS_TCP_SLAVE;

    ctx->backend = &_modbus_tcp_pi_backend;

    ctx->backend_data = (modbus_tcp_pi_t *) malloc(sizeof(modbus_tcp_pi_t));
    if (ctx->backend_data == NULL) {
        modbus_free(ctx);
        errno = ENOMEM;
        return NULL;
    }
    ctx_tcp_pi = (modbus_tcp_pi_t *) ctx->backend_data;
    ctx_tcp_pi->node = NULL;
    ctx_tcp_pi->service = NULL;

    if (node != NULL) {
        ctx_tcp_pi->node = strdup(node);
    } else {
        /* The node argument can be empty to indicate any hosts */
        ctx_tcp_pi->node = strdup("");
    }

    if (ctx_tcp_pi->node == NULL) {
        modbus_free(ctx);
        errno = ENOMEM;
        return NULL;
    }

    if (service != NULL && service[0] != '\0') {
        ctx_tcp_pi->service = strdup(service);
    } else {
        /* Default Modbus port number */
        ctx_tcp_pi->service = strdup("502");
    }

    if (ctx_tcp_pi->service == NULL) {
        modbus_free(ctx);
        errno = ENOMEM;
        return NULL;
    }

    ctx_tcp_pi->t_id = 0;

    return ctx;
}
