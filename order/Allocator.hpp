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

#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <map>
#include <mutex>
#include <vector>

#include "log.h"
#include "parson.h"
#include "Session.hpp"
#include "ServerConfig.hpp"
#include "UUID.hpp"

#include "../Algorithm.h"

// TODO: Reaper thread to expire sessions.
// TODO: Sessions ended before completion should be expired instead of ended.
// TODO: Use isValidOwner
// TODO: Should the server check to see if the allocator is valid?
// TODO: Fail allocation if a user has too many sessions are running.
// TODO: Tell the user object about its (gain or loss) of a session.

class Allocator {
    public:
        Allocator(const ServerConfig* serverConfig);
        ~Allocator();

        bool getNewSession(UUID &user_uuid, Session &session);
        bool getActiveSession(UUID &session_uuid, Session &session);
        bool endSession(UUID &session_uuid);
        bool expireActiveSession(UUID &session_uuid);
        bool expireAllActiveSessions();

        bool isSessionActive(const UUID &session_uuid) const;
        uint32_t getActiveSessionsForUser(const UUID &user_uuid);

        bool isValidOwner(const UUID &session_uuid, const UUID &user_uuid);
        bool disable();
        bool enable();

    private:
        void init();
        bool initNext();
        bool initSessions();
        bool isAllocatorEnabled() const;

        bool allocatorEnabled;
        std::mutex allocatorLock;
        const ServerConfig* serverConfig;

        Algorithm *nextStartAlg;
        unsigned int sessionSize;
        std::map<UUID, Session> sessionsActive;      /* Session UUID */
        std::map<UUID, Session> sessionsExpired;     /* Session UUID */
        std::map<UUID, uint32_t> userSessionsActive; /* User UUID    */

        JSON_Value*  nextRootValue  = NULL;
        JSON_Object* nextRootObject = NULL;

        bool swapSession(UUID session_uuid);
        bool activateRandomExpiredSession(const UUID &user_uuid, Session &session);

        const std::string next_schema =           \
        "{                                        \
            \"next_start_alg\": \"${ALGORITHM}\", \
            \"session_size\": ${NUM_ALGS}         \
        }";
};

#endif // ALLOCATOR_HPP
