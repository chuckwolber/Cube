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

#include <filesystem>

#include "log.h"
#include "parson.h"
#include "User.hpp"

User::User(const ServerConfig *serverConfig, const UUID &user_uuid) {
    UUID u_uuid(user_uuid);
    this->serverConfig = serverConfig;
    userFile = this->serverConfig->getUserFile(user_uuid);
    init(u_uuid);
}

bool User::isValidUser() const {
    return userExists() && isValid;
}

bool User::isEnabled() const {
    return (isValidUser() && role != Role::DISABLED);
}

bool User::isAdmin() const {
    return (isEnabled() && role == Role::ADMIN);
}

unsigned int User::getMaxActiveSessions() const {
    return maxActiveSessions;
}

unsigned int User::getCheckpointInterval() const {
    return checkpointInterval;
}

unsigned int User::getStrikes() const {
    return strikes;
}

unsigned int User::getMaxStrikes() const {
    return maxStrikes;
}

void User::init(UUID &user_uuid) {
    isValid = false;
    if (!userExists())
        return;

    JSON_Value *jv = json_parse_file(userFile.c_str());
    if (jv == NULL) {
        log_info("Failed to open user file: %s", userFile.c_str());
        return;
    }

    JSON_Object* obj = json_value_get_object(jv);
    if (obj == NULL) {
        log_info("Invalid configuration JSON.");
        json_value_free(jv);
        return;
    }

    user.setUUID(std::string(json_object_get_string(obj, "user")));
    name               = std::string(json_object_get_string(obj, "name"));
    email              = std::string(json_object_get_string(obj, "email"));
    role               = stringToRole(json_object_get_string(obj, "role"));
    maxActiveSessions  = (unsigned int)json_object_get_number(obj, "max_active_sessions");;
    checkpointInterval = (unsigned int)json_object_get_number(obj, "checkpoint_interval");;
    strikes            = (unsigned int)json_object_get_number(obj, "strikes");;
    maxStrikes         = (unsigned int)json_object_get_number(obj, "max_strikes");;
    json_value_free(jv);

    isValid = true;
    if (name.size() == 0       || email.size() == 0       || user.isNull()    ||
        maxActiveSessions == 0 || checkpointInterval == 0 || maxStrikes == 0  ||
        user != user_uuid)
        isValid = false;
}

bool User::userExists() const {
    if (!std::filesystem::exists(serverConfig->getUserDir(user)))
        return false;
    if (!std::filesystem::exists(userFile))
        return false;
    return true;
}

std::string User::roleToString(Role role) const {
    switch(role) {
        case DISABLED: {
            return "DISABLED";
            break;
        }
        case USER: {
            return "USER";
            break;
        }
        case ADMIN: {
            return "ADMIN";
            break;
        }
        default: {
            return "UNKNOWN";
        }
    }
}

User::Role User::stringToRole(std::string role) const {
    if (role == "DISABLED")
        return User::DISABLED;
    if (role == "USER")
        return User::USER;
    if (role == "ADMIN")
        return User::ADMIN;
    return User::UNKNOWN;
}
