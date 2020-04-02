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

/* See README.md for protocol and non-volatile storage definition. */

// TODO: CA validation - /usr/local/etc/openssl/cert.pem (macOS)
// TODO: Switch to std::mutex
// TODO: Admin UUID only allowed from localhost.

#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <thread>

#include "Allocator.hpp"
#include "Common.hpp"
#include "Results.hpp"
#include "Users.hpp"
#include "UUID.hpp"

#include "log.h"
#include "parson.h"

#include "../Algorithm.h"
#include "../Cube.h"

static const char* SERVER_VERSION = "0.0.1";
pthread_mutex_t MUTEX_LOG;
SSL_CTX *ctx = NULL;
int sock_fd;

Allocator* allocator;
Results* results;
ServerConfig* conf;
Users* users;

thread_local struct sockaddr_in thread_cli_addr;

bool init(const char* config_file);
bool init_mutexes();
void init_log();

void log_lock(bool lock, void* udata);
void socket_start();
void service_clients();
void* client_handler(int client_fd, struct sockaddr_in cli_addr);

void handle_request(SSL* ssl, int fd);
void req_start_session(SSL* ssl, int fd, UUID &user_uuid);
void req_restart_session(SSL* ssl, int fd, UUID &user_uuid);
void req_checkpoint(SSL* ssl, int fd, UUID &user_uuid);
void req_order_discovered(SSL* ssl, int fd, UUID &user_uuid);
void req_session_ended(SSL* ssl, int fd, UUID &user_uuid);

void req_admin(CMND_T cmnd, SSL* ssl, int fd, UUID &user_uuid);
void req_admin_expire(SSL* ssl, int fd, UUID &user_uuid);
void req_admin_expire_all(SSL* ssl, int fd, UUID &user_uuid);
void req_admin_disallow(SSL* ssl, int fd, UUID &user_uuid);
void req_admin_allow(SSL* ssl, int fd, UUID &user_uuid);
void req_admin_shutdown(SSL* ssl, int fd, UUID &user_uuid);

void req_invalid(SSL* ssl, int fd);

bool valid_preamble(SSL* ssl, int fd, UUID &user_uuid);
bool valid_version(SSL* ssl, int fd);
bool valid_user_uuid(SSL* ssl, int fd, UUID &user_uuid);
void short_reply(SSL* ssl, int fd, CODE_T code, RESN_T reason, RTRY_T retry);

void free_resources();

int main (int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN); /* Avoid SIGPIPE on short writes. */
    if (argc != 2 || !init(argv[1])) {
        fprintf(stderr, "ERROR: Initialization failure\n.");
        exit(1);
    }

    ctx = ssl_init(SERVER, conf->getTlsPubKeyFile().c_str(), 
                           conf->getTlsPrivKeyFile().c_str());
    socket_start();

    try {
        service_clients();
    } catch (std::runtime_error &err) {
        log_error("Caught runtime error. Check logs. Exiting...");
    }
    
    free_resources();

    return 0;
}

void handle_request(SSL* ssl, int fd) {
    UUID user_uuid;
    if (!valid_preamble(ssl, fd, user_uuid))
        return;

    uint8_t command;
    ssl_recv_n(ssl, &command, SZ_CMND_T);

    if (command == CMND_SES_NEW)
        req_start_session(ssl, fd, user_uuid);
    else if (command == CMND_SES_RESTART)
        req_restart_session(ssl, fd, user_uuid);
    else if (command == CMND_SES_PROGRESS)
        req_checkpoint(ssl, fd, user_uuid);
    else if (command == CMND_SES_ORDER)
        req_order_discovered(ssl, fd, user_uuid);
    else if (command == CMND_SES_END)
        req_session_ended(ssl, fd, user_uuid);
    else if (command == CMND_ADM_EXP      ||
             command == CMND_ADM_EXP_ALL  ||
             command == CMND_ADM_DISALLOW ||
             command == CMND_ADM_ALLOW    ||
             command == CMND_ADM_SHUTDOWN)
        req_admin(command, ssl, fd, user_uuid);
    else
        req_invalid(ssl, fd);
}

void req_start_session(SSL* ssl, int fd, UUID &user_uuid) {
    log_info("[%d] Session start request received for user %s", 
        fd, user_uuid.unparseUpper().c_str());
    // TODO - verify that allocator handles session limitations
    /*
    Session s;
    if (!allocator->getNewSession(user_uuid, s)) {
        short_reply(ssl, fd, CODE_FAIL_SES_NEW, RESN_EADM, 0);
        return;
    }

    std::string missingOrders = results->getMissingOrders();
    */

    short_reply(ssl, fd, CODE_OK_SES_NEW, 0, 0);
}

void req_restart_session(SSL* ssl, int fd, UUID &user_uuid) {
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_SES_RESTART, 0, 0);
}

void req_checkpoint(SSL* ssl, int fd, UUID &user_uuid) {
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_SES_PROGRESS, 0, 0);
}

void req_order_discovered(SSL* ssl, int fd, UUID &user_uuid) {
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_SES_ORDER, 0, 0);
}

void req_session_ended(SSL* ssl, int fd, UUID &user_uuid) {
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_SES_END, 0, 0);
}

void req_admin(CMND_T cmnd, SSL* ssl, int fd, UUID &user_uuid) {
    if (ntohl(thread_cli_addr.sin_addr.s_addr) != LOCALHOST ||
        !users->isAdminUser(user_uuid)) {
        log_info("[%d] Admin command invalid.", fd);
        short_reply(ssl, fd, CODE_FAIL_ADM_CMD, 0, 0);
        return;
    }

    if (cmnd == CMND_ADM_EXP)
        req_admin_expire(ssl, fd, user_uuid);
    else if (cmnd == CMND_ADM_EXP_ALL)
        req_admin_expire_all(ssl, fd, user_uuid);
    else if (cmnd == CMND_ADM_DISALLOW)
        req_admin_disallow(ssl, fd, user_uuid);
    else if (cmnd == CMND_ADM_ALLOW)
        req_admin_allow(ssl, fd, user_uuid);
    else if (cmnd == CMND_ADM_SHUTDOWN)
        req_admin_shutdown(ssl, fd, user_uuid);
}

void req_admin_expire(SSL* ssl, int fd, UUID &user_uuid) {
    log_info("[%d] Admin command - expire session.", fd);
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_ADM, 0, 0);
}

void req_admin_expire_all(SSL* ssl, int fd, UUID &user_uuid) {
    log_info("[%d] Admin command - expire all sessions.", fd);
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_ADM, 0, 0);
}

void req_admin_disallow(SSL* ssl, int fd, UUID &user_uuid) {
    log_info("[%d] Admin command - disallow new sessions.", fd);
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_ADM, 0, 0);
}

void req_admin_allow(SSL* ssl, int fd, UUID &user_uuid) {
    log_info("[%d] Admin command - allow new sessions.", fd);
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_ADM, 0, 0);
}

void req_admin_shutdown(SSL* ssl, int fd, UUID &user_uuid) {
    log_info("[%d] Admin command - shutdown server.", fd);
    (void)user_uuid;
    short_reply(ssl, fd, CODE_OK_ADM, 0, 0);
}

void req_invalid(SSL* ssl, int fd) {
    short_reply(ssl, fd, CODE_INVALID_CMND, 0, 0);
}

bool valid_preamble(SSL* ssl, int fd, UUID &user_uuid) {
    if (!valid_version(ssl, fd))
        return false;
    if (!valid_user_uuid(ssl, fd, user_uuid))
        return false;
    return true;
}

bool valid_version(SSL* ssl, int fd) {
    uint8_t ver[SZ_PVER_T];
    bool retval = true;

    if (ssl_recv_n(ssl, ver, SZ_PVER_T) < (int)SZ_PVER_T) { 
        log_error("[%d] Failed to receive Protocol Version...", fd);
        retval = false;
    } else if (*ver != PROTO_VERSION) {
        log_info("[%d] %s: %#04x", fd, CODE_STR_INVALID_PVER, *ver);
        retval = false;
    }

    if (!retval)
        short_reply(ssl, fd, CODE_INVALID_PVER, 0, 0);

    return retval;
}

bool valid_user_uuid(SSL* ssl, int fd, UUID &user_uuid) {
    bool retval = true;

    if (!ssl_recv_uuid(ssl, user_uuid))
        retval = false;
    else if (!users->isValidUser(user_uuid))
        retval = false;

    if (!retval) {
        short_reply(ssl, fd, CODE_INVALID_UUID_USR, 0, 0);
        log_info("[%d] Invalid User: %s", fd, user_uuid.unparseUpper().c_str());
    } else {
        log_info("[%d] Valid User: %s", fd, user_uuid.unparseUpper().c_str());
    }

    return retval;
}

void short_reply(SSL* ssl, int fd, CODE_T code, RESN_T reason, RTRY_T retry) {
    uint8_t reply_cd[SZ_CODE_T];
    uint8_t reply_rs[SZ_RESN_T];
    uint8_t reply_rt[SZ_RTRY_T];

    memcpy(reply_cd, &code,   SZ_CODE_T);
    memcpy(reply_rs, &reason, SZ_RESN_T);
    memcpy(reply_rt, &retry,  SZ_RTRY_T);
    
    if (ssl_send_n(ssl, reply_cd, (int)SZ_CODE_T) < (int)SZ_CODE_T)
        log_error("[%d] Failed to send result code: %#04x", fd, code);

    if (reason && ssl_send_n(ssl, reply_rs, (int)SZ_RESN_T) < (int)SZ_RESN_T)
        log_error("[%d] Failed to send result reason: %#04x", fd, reason);

    if (retry  && ssl_send_n(ssl, reply_rt, (int)SZ_RTRY_T) < (int)SZ_RTRY_T)
        log_error("[%d] Failed to send retry interval: %u", fd, retry);
}

void* client_handler(int client_fd, struct sockaddr_in cli_addr) {
    int ret_val;
    thread_cli_addr = cli_addr;
    
    SSL *ssl = SSL_new(ctx);
    if (ssl == NULL) {
        log_error("[%d] Failed to initiate TLS session.", client_fd);
        ssl_error();
        goto close_fd;
    }
    
    ret_val = SSL_set_fd(ssl, client_fd);
    if (ret_val <= 0) {
        log_error("[%d] Failed to bind socket to TLS session.", client_fd);
        ssl_io_error(ssl, ret_val);
        ssl_error();
        goto close_ssl;
    }

    ret_val = SSL_accept(ssl);
    if (ret_val <= 0) {
        log_error("[%d] TLS handshake failure - %d.", client_fd, ret_val);
        ssl_io_error(ssl, ret_val);
        ssl_error();
        goto close_ssl;
    }

    handle_request(ssl, client_fd);

close_ssl:
    if (SSL_shutdown(ssl) == 0)
        SSL_shutdown(ssl);
    SSL_free(ssl);
close_fd:
    close(client_fd);

    log_info("Client exiting...");

    return NULL; 
}

void service_clients() {
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);

    while (true) {
        int client_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addr_len);
        if (client_fd < 0) {
            log_error("Failed to accept client connection: %s", strerror(errno));
            sleep(1);
            continue;
        }

        char c_addr[INET_ADDRSTRLEN];
        log_info("[%d] Client Connected: %s:%d", client_fd,
            inet_ntop(AF_INET, &cli_addr.sin_addr, c_addr, 16),
            ntohs(cli_addr.sin_port));

        std::thread client_thread(client_handler, client_fd, cli_addr);
        client_thread.detach();
    }
}

void socket_start() {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        log_error("Failed to open socket: %s", strerror(errno));
        exit(1);
    }

    struct sockaddr_in sock_addr;
    memset((char*)&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_addr.sin_port = htons(conf->getTcpPort());

    if (bind(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
        log_error("Failed to bind to socket: %s", strerror(errno));
        close(sock_fd);
        exit(1);
    }

    if (listen(sock_fd, 5) < 0) {
        log_error("Failed to start listening to socket: %s", strerror(errno));
        close(sock_fd);
        exit(1);
    }

    log_info("Server Started...");
    log_info("Listening on port: %u", conf->getTcpPort());
}

bool init(const char* config_file) {
    conf = new ServerConfig(std::string(config_file));
    if (!conf->isConfigValid())
        return false;
    if (!init_mutexes())
        return false;

    init_log();
    try {
        allocator = new Allocator(conf);
        results = new Results(conf);
        users = new Users(conf);
    } catch (std::runtime_error &err) {
        return false;
    }

    /* Up to this point, logs appear on STDOUT. */
    log_set_quiet(true);

    return true;
}

bool init_mutexes() {
    errno = pthread_mutex_init(&MUTEX_LOG, NULL);
    if (errno < 0) {
        log_info("Failed to initialize MUTEX_LOG: [%d] %s", errno, strerror(errno));
        return false;
    }
    return true;
}

void init_log() {
    char *logFile = strdup(conf->getLogFile().c_str());
    rolling_appender ra = {logFile, conf->getMaxLogSize(), conf->getMaxLogs()};
    log_set_level(LOG_TRACE);
    log_set_lock(log_lock, &MUTEX_LOG);
    log_add_rolling_appender(ra, LOG_TRACE);
    log_info("Cube Server v%s Initializing...", SERVER_VERSION);
    conf->logConfig();
}

void log_lock(bool lock, void* udata) {
   pthread_mutex_t *LOCK = (pthread_mutex_t*)(udata);
    if (lock)
        pthread_mutex_lock(LOCK);
    else
        pthread_mutex_unlock(LOCK);
}

void free_resources() {
    SSL_CTX_free(ctx);
    close(sock_fd);
    pthread_mutex_destroy(&MUTEX_LOG);
    delete allocator;
    delete results;
    delete users;
    delete conf;
}
