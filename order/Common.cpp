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

#include "Common.hpp"

const char* CODE_STR_OK_SES_NEW       = "Session successfully assigned.";
const char* CODE_STR_OK_SES_RESTART   = "Session successfully restarted.";
const char* CODE_STR_OK_SES_PROGRESS  = "Session progress update successful.";
const char* CODE_STR_OK_SES_ORDER     = "Successfully reported novel order.";
const char* CODE_STR_OK_SES_END       = "Successfully ended session.";
const char* CODE_STR_OK_ADM           = "Administrative command completed successfully.";
const char* CODE_STR_INVALID_PVER     = "Invalid protocol version. Please update.";
const char* CODE_STR_INVALID_USR_UUID = "Invalid user UUID.";
const char* CODE_STR_INVALID_CODE     = "Invalid command code.";
const char* CODE_STR_INVALID_SES_UUID = "Invalid session UUID.";
const char* CODE_STR_INVALID_ANUM     = "Invalid algorithm number.";
const char* CODE_STR_FAIL_SES_NEW     = "Server failed to assign session.";
const char* CODE_STR_FAIL_SES_RESTART = "Server failed to restart session.";
const char* CODE_STR_TERMINATED_SES   = "Server terminated session.";
const char* CODE_STR_FAIL_ADM_CMD     = "Administrative command failed.";
const char* CODE_STR_INVALID          = "Invalid return value.";

static const char* env_keylog = "SSLKEYLOGFILE";
void key_callback(const SSL *ssl, const char *line) {
    char* keylog = getenv(env_keylog);
    (void)ssl;

    if (keylog != NULL) {
        log_info("SSL Keylog File: %s=%s", env_keylog, keylog);
        FILE *fp = fopen(keylog, "w");
        if (fp != NULL) {
            fprintf(fp, "%s", line);
            fclose(fp);
        }
    }
}

SSL_CTX* ssl_init(enum CS_TYPE type, const char* cert, const char* key) {
    SSL_CTX *ctx;
    const SSL_METHOD *method;

    if (type == CLIENT)
        method = TLS_client_method();
    else
        method = TLS_server_method();
    
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        log_error("Unable to create SSL context");
        exit(1);
    }
    
    if (SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION) <= 0) {
        log_error("Unable to set minimum TLS version");
        exit(1);
    }

    if (type == SERVER) {
        if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {
            log_error("Unable to open certificate: %s", cert);
            exit(1);
        }

        if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0 ) {
            log_error("Unable to private open key: %s", key);
            exit(1);
        }

        if (getenv(env_keylog))
            SSL_CTX_set_keylog_callback(ctx, &key_callback);
    }

    return ctx;
}

int ssl_send_n(SSL* ssl, uint8_t* buf, int len) {
    int cur_bytes_sent = 0;
    int tot_bytes_sent = 0;

    while (len > 0) {
        cur_bytes_sent = SSL_write(ssl, buf, len);
        tot_bytes_sent += cur_bytes_sent;
        if (cur_bytes_sent <= 0) {
            ssl_io_error(ssl, cur_bytes_sent);
            ssl_error();
            return -1;
        }
        len -= cur_bytes_sent;
        buf += cur_bytes_sent;
    }

    return tot_bytes_sent;
}

int ssl_recv_n(SSL* ssl, uint8_t* buf, int len) {
    int cur_bytes_received = 0;
    int tot_bytes_received = 0;

    while (len > 0) {
        cur_bytes_received = SSL_read(ssl, buf, len);
        tot_bytes_received += cur_bytes_received;
        if (cur_bytes_received <= 0) {
            ssl_io_error(ssl, (int)cur_bytes_received);
            ssl_error();
            return -1;
        }
        len -= cur_bytes_received;
        buf += cur_bytes_received;
    }

    return tot_bytes_received;
}

bool ssl_send_c_string(SSL* ssl, std::string &str) {
    SIZE_T strSize = (SIZE_T)str.length();
    size_t bufSize = sizeof(SZ_SIZE_T) + strSize;
    uint8_t* buf = (uint8_t*)malloc(bufSize);

    memcpy(buf, &strSize, SZ_SIZE_T);
    memcpy(buf + SZ_SIZE_T, buf, (size_t)strSize);
    
    int sent = ssl_send_n(ssl, buf, (int)bufSize);
    free(buf);

    if (sent != (int)bufSize)
        return false;
    return true;
}

bool ssl_recv_c_string(SSL* ssl, std::string &str) {
    int result;
    uint8_t* strSizeBuf = (uint8_t*)malloc(SZ_SIZE_T);

    result = ssl_recv_n(ssl, strSizeBuf, SZ_SIZE_T);
    if (result != SZ_SIZE_T) {
        free(strSizeBuf);
        return false;
    }

    SIZE_T strSize;
    memcpy(&strSize, strSizeBuf, SZ_SIZE_T);
    uint8_t* buf = (uint8_t*)calloc(strSize + 1, sizeof(uint8_t));
    free(strSizeBuf);
    
    result = ssl_recv_n(ssl, buf, (int)strSize);
    if (result != (int)strSize) {
        free(buf);
        return false;
    }

    str = (char*)buf;
    free(buf);
    
    return true;
}

bool ssl_send_uuid(SSL* ssl, UUID &uuid) {
    uint8_t buf[UUID::UUID_SIZE];
    uuid.copy(buf);

    if (ssl_send_n(ssl, buf, (int)UUID::UUID_SIZE) < (int)UUID::UUID_SIZE)
        return false;
    return true;
}

bool ssl_recv_uuid(SSL* ssl, UUID &uuid) {
    uint8_t buf[UUID::UUID_SIZE];
    if (ssl_recv_n(ssl, buf, (int)UUID::UUID_SIZE) < (int)UUID::UUID_SIZE)
        return false;

    uuid.setUUID(buf);
    return true;
}

void ssl_error() {
    unsigned long err;
    do {
        err = ERR_get_error();
        log_error("SSL ERROR - %s", ERR_error_string(err, NULL));
    } while (err != 0);
}

int ssl_io_error(const SSL* ssl, int ret) {
    int error = SSL_get_error(ssl, ret);
    switch(error) {
        case SSL_ERROR_NONE:
            log_error("SSL_ERROR_NONE");
            break;
        case SSL_ERROR_ZERO_RETURN:
            log_error("SSL_ERROR_ZERO_RETURN");
            break;
        case SSL_ERROR_WANT_READ:
            log_error("SSL_ERROR_WANT_READ");
            break;
        case SSL_ERROR_WANT_WRITE:
            log_error("SSL_ERROR_WANT_WRITE");
            break;
        case SSL_ERROR_WANT_CONNECT:
            log_error("SSL_ERROR_WANT_CONNECT");
            break;
        case SSL_ERROR_WANT_ACCEPT:
            log_error("SSL_ERROR_WANT_ACCEPT");
            break;
        case SSL_ERROR_WANT_X509_LOOKUP:
            log_error("SSL_ERROR_WANT_X509_LOOKUP");
            break;
        case SSL_ERROR_WANT_ASYNC:
            log_error("SSL_ERROR_WANT_ASYNC");
            break;
        case SSL_ERROR_WANT_ASYNC_JOB:
            log_error("SSL_ERROR_WANT_ASYNC_JOB");
            break;
        case SSL_ERROR_WANT_CLIENT_HELLO_CB:
            log_error("SSL_ERROR_WANT_CLIENT_HELLO_CB");
            break;
        case SSL_ERROR_SYSCALL:
            log_error("SSL_ERROR_SYSCALL: %s", strerror(errno));
            break;
        case SSL_ERROR_SSL:
            log_error("SSL_ERROR_SSL");
            break;
        default:
            log_error("GCC_MADE_ME_DO_IT");
            break;
    }

    return error;
}

const char* result_code_to_string(const uint16_t result_code) {
    if (result_code == CODE_OK_SES_NEW)
        return CODE_STR_OK_SES_NEW;
    if (result_code == CODE_OK_SES_RESTART)
        return CODE_STR_OK_SES_RESTART;
    if (result_code == CODE_OK_SES_PROGRESS)
        return CODE_STR_OK_SES_PROGRESS;
    if (result_code == CODE_OK_SES_ORDER)
        return CODE_STR_OK_SES_ORDER;
    if (result_code == CODE_OK_SES_END)
        return CODE_STR_OK_SES_END;
    if (result_code == CODE_OK_ADM)
        return CODE_STR_OK_ADM;
    if (result_code == CODE_INVALID_PVER)
        return CODE_STR_INVALID_PVER;
    if (result_code == CODE_INVALID_UUID_USR)
        return CODE_STR_INVALID_USR_UUID;
    if (result_code == CODE_INVALID_CMND)
        return CODE_STR_INVALID_CODE;
    if (result_code == CODE_INVALID_UUID_SES)
        return CODE_STR_INVALID_SES_UUID;
    if (result_code == CODE_INVALID_ANUM)
        return CODE_STR_INVALID_ANUM;
    if (result_code == CODE_FAIL_SES_NEW)
        return CODE_STR_FAIL_SES_NEW;
    if (result_code == CODE_FAIL_SES_RESTART)
        return CODE_STR_FAIL_SES_RESTART;
    if (result_code == CODE_TERMINATED_SES)
        return CODE_STR_TERMINATED_SES;
    if (result_code == CODE_FAIL_ADM_CMD)
        return CODE_STR_FAIL_ADM_CMD;
    return CODE_STR_INVALID;
}
