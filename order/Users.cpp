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

#include "parson.h"
#include "Users.hpp"

Users::Users(const ServerConfig* serverConfig) {
    this->serverConfig = serverConfig;
}

Users::~Users() {
    for (const std::pair<UUID, User*>& user : users)
        delete user.second;
}

bool Users::isValidUser(const UUID &user_uuid) {
    if (!cacheUser(user_uuid))
        return false;
    return users[user_uuid]->isValidUser();
}

bool Users::isAdminUser(const UUID &user_uuid) {
    if (!cacheUser(user_uuid))
        return false;
    return users[user_uuid]->isAdmin();
}

bool Users::isEnabledUser(const UUID &user_uuid) {
    if (!cacheUser(user_uuid))
        return false;
    return users[user_uuid]->isEnabled();
}

bool Users::cacheUser(const UUID &user_uuid) {
    if (users.find(user_uuid) != users.end())
        return true;

    User* user = new User(serverConfig, user_uuid);
    if (user->isValidUser()) {
        users[user_uuid] = user;
        return true;
    } else {
        delete user;
        return false;
    }
}
