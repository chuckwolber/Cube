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

#include "ServerConfig.hpp"
#include "log.h"

ServerConfig::ServerConfig(std::string serverConfigFile) {
    this->serverConfigFile = serverConfigFile;
    initConfig();
}

bool ServerConfig::isConfigValid() const {
    return isValid;
}

double ServerConfig::getSessionSize() const {
    return sessionSize;
}

unsigned int ServerConfig::getMaxLogs() const {
    return maxLogs;
}

off_t ServerConfig::getMaxLogSize() const {
    return maxLogSize;
}

uint16_t ServerConfig::getTcpPort() const {
    return tcpPort;
}

std::string ServerConfig::getTlsPubKeyFile() const {
    return tlsPubKey;
}

std::string ServerConfig::getTlsPrivKeyFile() const {
    return tlsPrivKey;
}

std::string ServerConfig::getLogFile() const {
    return logFile;
}

std::string ServerConfig::getDataRootDir() const {
    return dataRoot;
}

std::string ServerConfig::getResultsDir() const {
    return getDataRootDir() + "/" + resultsDir;
}

std::string ServerConfig::getSessionsDir() const {
    return getDataRootDir() + "/" + sessionsDir;
}

std::string ServerConfig::getUsersDir() const {
    return getDataRootDir() + "/" + usersDir;
}

std::string ServerConfig::getAllSessionsDir() const {
    return getSessionsDir() + "/" + allSessionsDir;
}

std::string ServerConfig::getExpiredSessionsDir() const {
    return getSessionsDir() + "/" + expiredSesionsDir;
}

std::string ServerConfig::getIncompleteSessionsDir() const {
    return getSessionsDir() + "/" + incompleteSessionsDir;
}

std::string ServerConfig::getNextFile() const {
    return getIncompleteSessionsDir() + "/" + nextJson;
}

std::string ServerConfig::getUserDir(UUID user) const {
    return getDataRootDir() + "/" + usersDir + "/" + user.unparseUpper();
}

std::string ServerConfig::getUserFile(UUID user) const {
    return getUserDir(user) + "/" + userJson;
}

std::string ServerConfig::getResultsLink(UUID user) const {
    return getResultsDir() + "/" + user.unparseUpper();
}

std::string ServerConfig::getResultsFile(UUID user) const {
    return getUserDir(user) + "/" + resultsJson;
}

std::string ServerConfig::getSessionDir(UUID session) const {
    return getAllSessionsDir() + "/" + session.unparseUpper();
}

std::string ServerConfig::getSessionFile(UUID session) const {
    return getSessionDir(session) + "/" + sessionJson;
}

std::string ServerConfig::getSessionDir(UUID user, UUID session) const {
    return getUserDir(user) +
                    + "/" + sessionsDir
                    + "/" + session.unparseUpper();
}

std::string ServerConfig::getSessionFile(UUID user, UUID session) const {
    return getSessionDir(user, session) + "/" + sessionJson;
}

std::string ServerConfig::getIncompleteSessionDir(UUID session) const {
    return getIncompleteSessionsDir() + "/" + session.unparseUpper();
}

void ServerConfig::initConfig() {
    JSON_Value* jv = json_parse_file(serverConfigFile.c_str());
    if (jv == NULL) {
        log_info("Failed to open config file: %s", serverConfigFile.c_str());
        isValid = false;
        return;
    }

    JSON_Object* obj = json_value_get_object(jv);
    if (obj == NULL) {
        log_info("Invalid configuration JSON.");
        isValid = false;
        json_value_free(jv);
        return;
    }

    double port = 0;

    sessionSize = (unsigned int)json_object_get_number(obj, "session_size");
    dataRoot    = getConfigString(obj, "data_root");
    port        = (uint16_t)json_object_dotget_number(obj, "network.tcp_port");
    tlsPubKey   = getConfigString(obj, "network.tls_pub_key");
    tlsPrivKey  = getConfigString(obj, "network.tls_priv_key");
    logFile     = getConfigString(obj, "logging.log_file");
    maxLogSize  = (off_t)json_object_dotget_number(obj, "logging.max_log_size");
    maxLogs     = (unsigned int)json_object_dotget_number(obj, "logging.max_logs");

    json_value_free(jv);

    if (port >= TCP_PORT_MIN && port <= TCP_PORT_MAX)
        tcpPort = (uint16_t)port;

    isValid = true;
    if (sessionSize      == 0  || dataRoot.size()   == 0 || tcpPort == 0 ||
        tlsPubKey.size() == 0  || tlsPrivKey.size() == 0 ||
        logFile.size()   == 0  || maxLogSize        == 0 || maxLogs == 0)
        isValid = false;
}

std::string ServerConfig::getConfigString(JSON_Object *obj, std::string dotString) const {
    const char* st = json_object_dotget_string(obj, dotString.c_str());
    if (!st) {
        log_info("Missing configuration directive: %s", dotString.c_str());
        return "";
    }

    std::string result(st);
    return result;
}

void ServerConfig::logConfig() const {
    log_info("   ****** Server Configuration ******");
    log_info("        session_size: %u", sessionSize);
    log_info("           data_root: %s", dataRoot.c_str());
    log_info("    network.tcp_port: %u", tcpPort);
    log_info(" network.tls_pub_key: %s", tlsPubKey.c_str());
    log_info("network.tls_priv_key: %s", tlsPrivKey.c_str());
    log_info("    logging.log_file: %s", logFile.c_str());
    log_info("    logging.max_logs: %u", maxLogs);
    log_info("logging.max_log_size: %jd", maxLogSize);
}
