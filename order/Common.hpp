/**
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2020 Chuck Wolber
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

// TODO: common recv string needs to have an upper limit.

#ifndef COMMON_HPP
#define COMMON_HPP

#include "log.h"
#include "UUID.hpp"

#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// TODO: Configuration file level max receive size and a hard coded absolute max receive size.

enum CS_TYPE {CLIENT, SERVER};
enum ADM_CMND { ALLOW, DISALLOW, EXPIRE, SHUTDOWN, UNKNOWN };

/* Field Types */
typedef uint32_t ANUM_T;
typedef uint8_t  CMND_T;
typedef uint16_t CODE_T;
typedef uint16_t PROC_T;
typedef uint8_t  PVER_T;
typedef uint8_t  RESN_T;
typedef uint32_t RTRY_T;
typedef uint32_t SIZE_T;

static const PVER_T PROTO_VERSION = 0x01;
static const uint32_t LOCALHOST = 0x7f000001;

/* Command Codes */
static const CMND_T CMND_SES_NEW      = 0x01;
static const CMND_T CMND_SES_RESTART  = 0x02;
static const CMND_T CMND_SES_PROGRESS = 0x03;
static const CMND_T CMND_SES_ORDER    = 0x04;
static const CMND_T CMND_SES_END      = 0x05;
static const CMND_T CMND_ADM_EXP      = 0xA0;
static const CMND_T CMND_ADM_EXP_ALL  = 0xA1;
static const CMND_T CMND_ADM_DISALLOW = 0xA2;
static const CMND_T CMND_ADM_ALLOW    = 0xA3;
static const CMND_T CMND_ADM_SHUTDOWN = 0xAF;

/* Field Sizes */
static const size_t SZ_ANUM_T = sizeof(ANUM_T);
static const size_t SZ_CMND_T = sizeof(CMND_T);
static const size_t SZ_CODE_T = sizeof(CODE_T);
static const size_t SZ_PROC_T = sizeof(PROC_T);
static const size_t SZ_PVER_T = sizeof(PVER_T);
static const size_t SZ_RESN_T = sizeof(RESN_T);
static const size_t SZ_RTRY_T = sizeof(RTRY_T);
static const size_t SZ_SIZE_T = sizeof(SIZE_T);

/* Request Sizes */
static const size_t SZ_PREAMBLE     = SZ_PVER_T + UUID::UUID_SIZE + SZ_CMND_T;
static const size_t SZ_SES_NEW      = SZ_PREAMBLE + SZ_PROC_T;
static const size_t SZ_SES_RESTART  = SZ_PREAMBLE + UUID::UUID_SIZE; 
static const size_t SZ_SES_PROGRESS = SZ_PREAMBLE + UUID::UUID_SIZE + SZ_ANUM_T;
static const size_t SZ_SES_ORDER    = SZ_PREAMBLE + UUID::UUID_SIZE + SZ_ANUM_T;
static const size_t SZ_SES_END      = SZ_PREAMBLE + UUID::UUID_SIZE + SZ_ANUM_T;
static const size_t SZ_ADM_EXP      = SZ_PREAMBLE + UUID::UUID_SIZE;
static const size_t SZ_ADM_EXP_ALL  = SZ_PREAMBLE;
static const size_t SZ_ADM_DISALLOW = SZ_PREAMBLE + SZ_RTRY_T;
static const size_t SZ_ADM_ALLOW    = SZ_PREAMBLE;
static const size_t SZ_ADM_SHUTDOWN = SZ_PREAMBLE;

/* Request Result Codes */
static const CODE_T CODE_OK_SES_NEW       = 0x0101;
static const CODE_T CODE_OK_SES_RESTART   = 0x0201;
static const CODE_T CODE_OK_SES_PROGRESS  = 0x0301;
static const CODE_T CODE_OK_SES_ORDER     = 0x0401;
static const CODE_T CODE_OK_SES_END       = 0x0501;
static const CODE_T CODE_OK_ADM           = 0xAC01;
static const CODE_T CODE_INVALID_CMND     = 0xFF00;
static const CODE_T CODE_INVALID_PVER     = 0xFF01;
static const CODE_T CODE_INVALID_UUID_USR = 0xFF02;
static const CODE_T CODE_INVALID_UUID_SES = 0xFF03;
static const CODE_T CODE_INVALID_ANUM     = 0xFF04;
static const CODE_T CODE_FAIL_SES_NEW     = 0xFF80;
static const CODE_T CODE_FAIL_SES_RESTART = 0xFF81;
static const CODE_T CODE_TERMINATED_SES   = 0xFF82;
static const CODE_T CODE_FAIL_ADM_CMD     = 0xFF83;

/* Request Result Reason Codes */
static const RESN_T RESN_NONE = 0x00; /* No Reason code given.             */
static const RESN_T RESN_COMP = 0x01; /* Project complete :)               */
static const RESN_T RESN_NSES = 0x02; /* No sessions available.            */
static const RESN_T RESN_UMAX = 0x03; /* User session limit exceeded.      */
static const RESN_T RESN_EXPR = 0x04; /* Session expired.                  */
static const RESN_T RESN_OINV = 0x05; /* Discovered order was invalid.     */
static const RESN_T RESN_EADM = 0x06; /* Session expired administratively. */

/* Request Result Code Strings */
extern const char* CODE_STR_OK_SES_NEW;
extern const char* CODE_STR_OK_SES_RESTART;
extern const char* CODE_STR_OK_SES_PROGRESS;
extern const char* CODE_STR_OK_SES_ORDER;
extern const char* CODE_STR_OK_SES_END;
extern const char* CODE_STR_OK_ADM;
extern const char* CODE_STR_INVALID_PVER;
extern const char* CODE_STR_INVALID_USR_UUID;
extern const char* CODE_STR_INVALID_CODE;
extern const char* CODE_STR_INVALID_SES_UUID;
extern const char* CODE_STR_INVALID_ANUM;
extern const char* CODE_STR_FAIL_SES_NEW;
extern const char* CODE_STR_FAIL_SES_RESTART;
extern const char* CODE_STR_TERMINATED_SES;
extern const char* CODE_STR_FAIL_ADM_CMD;
extern const char* CODE_STR_INVALID;

SSL_CTX* ssl_init(enum CS_TYPE type, const char* cert, const char* key);

int ssl_send_n(SSL* ssl, uint8_t* buf, int len);
int ssl_recv_n(SSL* ssl, uint8_t* buf, int len);

/* Send and received C strings with a 32 bit size preamble. */
bool ssl_send_c_string(SSL* ssl, std::string &str);
bool ssl_recv_c_string(SSL* ssl, std::string &str);

bool ssl_send_uuid(SSL* ssl, UUID &uuid);
bool ssl_recv_uuid(SSL* ssl, UUID &uuid);

void ssl_error();
int ssl_io_error(const SSL* ssl, int ret);
const char* result_code_to_string(const uint16_t result_code);

#endif // COMMON_HPP
