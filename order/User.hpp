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

#ifndef USER_HPP
#define USER_HPP

#include <map>

#include "ServerConfig.hpp"
#include "Session.hpp"
#include "UUID.hpp"

class User {
    public:
        User(const ServerConfig *serverConfig, const UUID &user_uuid);

        bool isValidUser() const;
        bool isAdmin() const;
        bool isEnabled() const;

        unsigned int getMaxActiveSessions() const;
        unsigned int getCheckpointInterval() const;
        unsigned int getStrikes() const;
        unsigned int getMaxStrikes() const;

    private:
        enum Role { DISABLED, USER, ADMIN, UNKNOWN };
        bool userExists() const;
        void init(UUID &user_uuid);

        const ServerConfig *serverConfig;
        std::map<UUID, Session> sessions;
        std::string userFile;

        bool isValid;
        std::string name;
        std::string email;
        UUID user;
        Role role;
        unsigned int maxActiveSessions;
        unsigned int checkpointInterval;
        unsigned int strikes;
        unsigned int maxStrikes;

        std::string roleToString(Role role) const;
        Role stringToRole(std::string role) const;

        const std::string userSchema =                \
        "{                                            \
            \"name\": \"${NAME}\",                    \
            \"email\": \"${EMAILADDRESS}\",           \
            \"user\": \"${USER}\",                    \
            \"role\": \"${ROLE}\",                    \
            \"max_active_sessions\": ${MAX_SESSIONS}, \
            \"checkpoint_interval\": ${SECONDS}       \
            \"strikes\": ${STRIKES},                  \
            \"max_strikes\": ${STRIKES}               \
        }";
};

#endif // USER_HPP
