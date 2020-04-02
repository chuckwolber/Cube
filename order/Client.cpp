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

#include <getopt.h>
#include <netdb.h>
#include <string.h>

#include <thread>

#include "Common.hpp"
#include "UUID.hpp"

#include "../Algorithm.h"
#include "../Cube.h"

// TODO: Exclude hyperthreads in core count. CPU affinity may do this.
// TODO: Command to get user's stats.

SSL_CTX* ctx;
SSL *ssl;
int sock_fd;

uint8_t* PREAMBLE;
char* SERVER_HOST;
ADM_CMND ADMIN_CMD;
RTRY_T RETRY_INTERVAL;
PROC_T NUM_PROCS;
uint16_t PORT;
UUID USER_UUID;
UUID SESSION_UUID;

void server_connect();
void close_ssl();
void close_socket();

CODE_T send_command(CMND_T command, uint8_t* s_buf, size_t s_buf_size);
void init_preamble();

void client_ops();
void start_session();
void restart_session();
void admin_command();
void admin_command_expire_session();
void admin_command_expire_all();
void admin_command_disallow();
void admin_command_allow();
void admin_command_shutdown();

bool set_args(int argc, char** argv);
void usage(const char* exec, const char* message);
ADM_CMND get_cmd_for_string(char* cmnd);

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN); /* Avoid SIGPIPE on short writes. */
    if (!set_args(argc, argv))
        return 1;

    init_preamble();

    log_info("Client Started...");
    client_ops();
    log_info("Client Exiting...");

    return 0;
}

void client_ops() {
    if (ADMIN_CMD != ADM_CMND::UNKNOWN)
        admin_command();
    else if (!SESSION_UUID.isNull())
        restart_session();
    else
        start_session();
}

void admin_command() {
    if (ADMIN_CMD == ADM_CMND::EXPIRE || !SESSION_UUID.isNull())
        admin_command_expire_session();
    else if (ADMIN_CMD == ADM_CMND::EXPIRE)
        admin_command_expire_all();
    else if (ADMIN_CMD == ADM_CMND::DISALLOW)
        admin_command_disallow();
    else if (ADMIN_CMD == ADM_CMND::ALLOW)
        admin_command_allow();
    else if (ADMIN_CMD == ADM_CMND::SHUTDOWN)
        admin_command_shutdown();
}

void admin_command_expire_session() {
    uint8_t s_buf[UUID::UUID_SIZE];
    memcpy(s_buf, SESSION_UUID.unparseUpper().c_str(), UUID::UUID_SIZE);

    CODE_T code = send_command(CMND_ADM_EXP, s_buf, UUID::UUID_SIZE);
    log_info("%s", result_code_to_string(code));

    if (code) {
        close_ssl();
        close_socket();
    }
}

void admin_command_expire_all() {
    CODE_T code = send_command(CMND_ADM_EXP, NULL, 0);
    log_info("%s", result_code_to_string(code));

    if (code) {
        close_ssl();
        close_socket();
    }
}

void admin_command_disallow() {
    uint8_t s_buf[SZ_RTRY_T];
    memcpy(s_buf, &RETRY_INTERVAL, SZ_RTRY_T);

    CODE_T code = send_command(CMND_ADM_DISALLOW, s_buf, SZ_RTRY_T);
    log_info("%s", result_code_to_string(code));

    if (code) {
        close_ssl();
        close_socket();
    }
}

void admin_command_allow() {
    CODE_T code = send_command(CMND_ADM_ALLOW, NULL, 0);
    log_info("%s", result_code_to_string(code));

    if (code) {
        close_ssl();
        close_socket();
    }
}

void admin_command_shutdown() {
    CODE_T code = send_command(CMND_ADM_SHUTDOWN, NULL, 0);
    log_info("%s", result_code_to_string(code));

    if (code) {
        close_ssl();
        close_socket();
    }
}

void restart_session() {
}

void start_session() {
    uint8_t s_buf[SZ_PROC_T];
    memcpy(s_buf, &NUM_PROCS, SZ_PROC_T);

    CODE_T code = send_command(CMND_SES_NEW, s_buf, SZ_PROC_T);
    log_info("%s", result_code_to_string(code));

    if (code) {
        close_ssl();
        close_socket();
    }
}

void init_preamble() {
    uuid_t user_uuid;
    USER_UUID.copy(user_uuid);
    PREAMBLE = (uint8_t*)calloc(SZ_PREAMBLE, sizeof(uint8_t));
    memcpy(PREAMBLE, &PROTO_VERSION, SZ_PVER_T);
    memcpy(PREAMBLE + SZ_PVER_T, user_uuid, UUID::UUID_SIZE);
}

CODE_T send_command(CMND_T command, uint8_t* s_buf, size_t s_buf_size) {
    server_connect();
    if (sock_fd < 0)
        return 0x00;

    log_info("[%d] Connected to server...", sock_fd);

    ctx = ssl_init(CLIENT, NULL, NULL);
    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        log_error("[%d] Failed to initiate TLS session.", sock_fd);
        ssl_error();
        close_socket();
        return 0x00;
    }

    int ret_val = SSL_set_fd(ssl, sock_fd);
    if (ret_val <= 0) {
        log_error("[%d] Failed to bind socket to TLS session.", sock_fd);
        ssl_io_error(ssl, ret_val);
        ssl_error();
        close_ssl();
        close_socket();
        return 0x00;
    }

    SSL_set_connect_state(ssl);

    uint8_t buf[SZ_PREAMBLE];
    memcpy(buf, PREAMBLE, SZ_PREAMBLE);
    memcpy(buf + SZ_PREAMBLE - SZ_CMND_T, &command, SZ_CMND_T);
    if (ssl_send_n(ssl, buf, (int)SZ_PREAMBLE) != (int)SZ_PREAMBLE) {
        close_ssl();
        close_socket();
        return 0x00;
    }

    if (s_buf_size > 0 && ssl_send_n(ssl, s_buf, (int)s_buf_size) != (int)s_buf_size) {
        close_ssl();
        close_socket();
        return 0x00;
    }
    
    uint8_t buf_code[SZ_CODE_T];
    if (ssl_recv_n(ssl, buf_code, (int)SZ_CODE_T) < (int)SZ_CODE_T) {
        close_ssl();
        close_socket();
        return 0x00;
    }

    CODE_T code;
    memcpy(&code, buf_code, SZ_CODE_T);
    return code;    
}

void close_ssl() {
    if (SSL_shutdown(ssl) == 0)
        SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

void close_socket() {
    close(sock_fd);
    sock_fd = 0;
}

void server_connect() {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        log_error("Failed to open socket: %s", strerror(errno));
        return;
    }

    struct hostent *server = gethostbyname(SERVER_HOST);
    if (server == NULL) {
        log_error("Failed to get hostent: %s", strerror(errno));
        close(sock_fd);
        sock_fd = -1;
        return;
    }
    
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    memcpy((char*)&sock_addr.sin_addr.s_addr, (char*)server->h_addr, (size_t)server->h_length);
    sock_addr.sin_port = htons(PORT);

    if (connect(sock_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        log_error("Failed to connect to server: %s", strerror(errno));
        sock_fd = -1;
        return;
    }
}

bool set_args(int argc, char **argv) {
    int opt_char;
	int option_index;

    static struct option long_options[] = {
        {"host",           required_argument, 0, 'h'},
        {"port",           required_argument, 0, 'p'},
        {"user-uuid",      required_argument, 0, 'u'},
        {"num-procs",      required_argument, 0, 'n'},
        {"session-uuid",   required_argument, 0, 's'},
        {"admin",          required_argument, 0, 'a'},
        {"retry-interval", required_argument, 0, 'r'},
        {"help",           optional_argument, 0, 'x'},
        {0,                0,                 0,  0 }
    };

    USER_UUID.clear();
    SESSION_UUID.clear();
    ADMIN_CMD = ADM_CMND::UNKNOWN;
    RETRY_INTERVAL = 0;

    while((opt_char = getopt_long(argc, argv, "h:p:u:n:s:a",
				      long_options, &option_index)) != -1) {
        switch(opt_char) {
            case 'h': {
                SERVER_HOST = strdup(optarg);
                struct hostent *server = gethostbyname(SERVER_HOST);
                if (server == NULL) {
                    usage(argv[0], "Invalid server host address!");
                    return false;
                }
                break;
            }
            case 'p': {
                PORT = (uint16_t)atoi(optarg);
                break;
            }
            case 'u': {
                USER_UUID.setUUID(optarg);
                if (USER_UUID.isNull()) {
                    usage(argv[0], "Invalid user UUID.");
                    return false;
                }
                break;
            }
            case 'n': {
                NUM_PROCS = (uint16_t)atoi(optarg);
                uint16_t maxThreads = (uint16_t)std::thread::hardware_concurrency();
                if (NUM_PROCS > maxThreads || NUM_PROCS == 0)
                    NUM_PROCS = maxThreads;
                break;
            }
            case 's': {
                SESSION_UUID.setUUID(optarg);
                if (SESSION_UUID.isNull()) {
                    usage(argv[0], "Invalid session UUID.");
                    return false;
                }
                break;
            }
            case 'a': {
                ADMIN_CMD = get_cmd_for_string(optarg);
                if (ADMIN_CMD == ADM_CMND::UNKNOWN)
                    return false;
                break;
            }
            case 'r': {
                RETRY_INTERVAL = (RTRY_T)atoi(optarg);
                break;
            }
            default: {
                usage(argv[0], NULL);
                return false;
            }
        }
    }

    if (SERVER_HOST == NULL) {
        usage(argv[0], "Missing server host address.");
        return false;
    } else if (PORT <= 0) {
        usage(argv[0], "Missing port number.");
        return false;
    } else if (USER_UUID.isNull()) {
        usage(argv[0], "Missing user UUID.");
        return false;
    } else if (ADMIN_CMD == ADM_CMND::DISALLOW && RETRY_INTERVAL == 0) {
        return false;
    }
    
    unsigned int max_procs = std::thread::hardware_concurrency();
    if (NUM_PROCS == 0 || NUM_PROCS > max_procs)
        NUM_PROCS = (uint16_t)max_procs;

    return true;
}

void usage(const char* exec, const char* message) {
    if (message != NULL)
        log_error("%s", message);
    
    log_info("usage: %s --host host --port port --user-uuid uuid [--num-procs procs] [--session-uuid uuid]", exec);
}

ADM_CMND get_cmd_for_string(char* cmnd) {
    std::string command(cmnd);
    if (command == "ALLOW")
        return ADM_CMND::ALLOW;
    if (command == "DISALLOW")
        return ADM_CMND::DISALLOW;
    if (command == "EXPIRE")
        return ADM_CMND::EXPIRE;
    if (command == "SHUTDOWN")
        return ADM_CMND::SHUTDOWN;

    return ADM_CMND::UNKNOWN;
}
