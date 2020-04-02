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

/**
 * With the exception of the no-argument constructor, Session objects are
 * managed by the Allocator in order to avoid race conditions.
 * 
 * This class assumes user validation has been done
 */

// TODO: the equivalent of a mkdir -p when creating session files

#ifndef SESSION_HPP
#define SESSION_HPP

#include "parson.h"
#include "ServerConfig.hpp"
#include "UUID.hpp"

class Session {
    public:
        enum SessionState {
            ACTIVE,
            EXPIRED,
            COMPLETE,
            UNKNOWN
        };
        
        Session();
        Session(const Session& obj);
        Session& operator=(const Session& rhs);
        bool operator==(const Session& rhs) const;

        void setConfig(const ServerConfig* serverConfig);
        bool readSession(UUID session);
        bool newSession(UUID user, std::string algorithm, unsigned int size);

        bool setUser(UUID user);
        bool setSessionState(SessionState state);

        UUID getUser();
        UUID getSession();
        SessionState getSessionState();
        unsigned int getNumProcessors();
        std::string getAlgorithm();
        unsigned int getSize();

        static std::string sessionStateToString(SessionState state);
        static SessionState stringToSessionState(std::string state);

    private:
        bool sessionOwned = false;
        
        const ServerConfig *serverConfig;
        std::string sessionFile;

        UUID user;
        UUID session;
        SessionState state    = UNKNOWN;
        unsigned int nproc    = 0;
        std::string algorithm = "";
        unsigned int size     = 0;

        bool isSessionValid();
        bool isSessionNew(UUID user);
        bool sessionFileExists(UUID user);
        bool sessionsLinkExists();
        bool createSessionsLink(UUID user);
        bool createIncompleteSessionsLink(UUID user);

        JSON_Value*  sessionRootValue  = NULL;
        JSON_Object* sessionRootObject = NULL;
        bool persistSession();
        bool parseSessionRootValue();
        bool parseSessionRootObject();
        bool parseUser();
        bool parseSession();
        bool parseSessionState();
        bool parseAlgorithm();
        bool parseSize();

        const std::string sessionSechema =                  \
        "{                                                  \
            \"user\": \"${USER}\",                          \
            \"session\": \"${SESSION}\",                    \
            \"dateTimeStarted\": \"${DATETIME}\",           \
            \"checkpointLast\": \"${DATETIME}\",            \
            \"dateTimeEnded\": \"${DATETIME}\",             \
            \"state\": \"${SESSION_STATE}\",                \
            \"nproc\": ${NUM_PROCS},                        \
            \"start_alg\": \"${ALGORITHM}\",                \
            \"session_size\": ${NUM_ALGS},                  \
        }";
};

#endif // SESSION_HPP
