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

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>

#include "parson.h"
#include "UUID.hpp"

class ServerConfig {
    public:
        ServerConfig() = default;
        ServerConfig(std::string serverConfigFile);

        ServerConfig(const ServerConfig& obj) = delete;
        ServerConfig& operator=(const ServerConfig& rhs) = delete;

        void logConfig()            const;
        bool isConfigValid()        const;

        double getSessionSize()     const;
        unsigned int getMaxLogs()   const;
        off_t getMaxLogSize()       const;
        uint16_t getTcpPort()       const;

        std::string getTlsPubKeyFile()           const;
        std::string getTlsPrivKeyFile()          const;
        std::string getLogFile()                 const;

        std::string getDataRootDir()             const;
        std::string getResultsDir()              const;
        std::string getSessionsDir()             const;
        std::string getUsersDir()                const;
        std::string getAllSessionsDir()         const;
        std::string getExpiredSessionsDir()      const;
        std::string getIncompleteSessionsDir()   const;
        std::string getNextFile()                const;
        std::string getUserDir(UUID user)        const;
        std::string getUserFile(UUID user)       const;
        std::string getResultsLink(UUID user)    const;
        std::string getResultsFile(UUID user)    const;
        std::string getSessionDir(UUID session)  const;
        std::string getSessionFile(UUID session) const;
        std::string getSessionDir(UUID user, UUID session)  const;
        std::string getSessionFile(UUID user, UUID session) const;
        std::string getIncompleteSessionDir(UUID session)   const;

    private:
        void copyObj(const ServerConfig &obj);
        void initConfig();
        std::string getConfigString(JSON_Object *obj, std::string dotString) const;

        std::string dataRoot;
        std::string serverConfigFile;

        const std::string resultsDir            = "results";
        const std::string sessionsDir           = "sessions";
        const std::string allSessionsDir        = "all";
        const std::string expiredSesionsDir     = "expire";
        const std::string incompleteSessionsDir = "incomplete";
        const std::string usersDir              = "users";

        const std::string nextJson      = "next.json";
        const std::string userJson      = "user.json";
        const std::string resultsJson   = "results.json";
        const std::string sessionJson   = "session.json";
        const std::string checkpointLog = "checkpoint.log";

        bool isValid             = false;
        unsigned int sessionSize = 0;
        uint16_t tcpPort         = 0;
        std::string tlsPubKey    = "";
        std::string tlsPrivKey   = "";
        std::string logFile      = "";
        off_t maxLogSize         = 0;
        unsigned int maxLogs     = 0;

        const uint16_t TCP_PORT_DEFAULT = 32125;
        const uint16_t TCP_PORT_MIN = 1024;
        const uint16_t TCP_PORT_MAX = 49152;

        const std::string serverConfigSchema =     \
        "{                                         \
            \"session_size\": \"${NUM_ALGS}\",     \
            \"data_root\": \"${DATA_ROOT}\",       \
            \"network\": {                         \
                \"tcp_port\": ${PORT},             \
                \"tls_pub_key\": \"${CERT}\",      \
                \"tls_priv_key\": \"${CERT}\"      \
            },                                     \
            \"logging\": {                         \
                \"log_file\": \"${LOG_FILE}\",     \
                \"max_log_size\": ${MAX_LOG_SIZE}, \
                \"max_logs\": ${MAX_LOGS}          \
            }                                      \
        }";
};

#endif // SERVERCONFIG_HPP
