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

#include "Allocator.hpp"
#include "log.h"

Allocator::Allocator(const ServerConfig* serverConfig) {
    this->serverConfig = serverConfig;
    allocatorEnabled = false;
    init();
}

Allocator::~Allocator() {
    if (nextStartAlg != NULL)
        delete nextStartAlg;
    if (nextRootValue != NULL)
        json_value_free(nextRootValue);
}

bool Allocator::getNewSession(UUID &user_uuid, Session &session) {
    std::lock_guard<std::mutex> lock(allocatorLock);
    if (!isAllocatorEnabled())
        return false;

    if (activateRandomExpiredSession(user_uuid, session))
        return true;

    Session newSession;
    newSession.setConfig(serverConfig);
    if (!newSession.newSession(user_uuid, nextStartAlg->getAlgorithmStr(), sessionSize))
        return false;

    sessionsActive[newSession.getSession()] = newSession;
    userSessionsActive[user_uuid]++;
    session = newSession;

    *(nextStartAlg) += sessionSize;
    json_object_set_string(nextRootObject, "next_start_alg", nextStartAlg->getAlgorithmStr().c_str());
    if (JSONFailure == json_serialize_to_file_pretty(nextRootValue, serverConfig->getNextFile().c_str())) {
        log_error("Failed to serialize JSON: %s", serverConfig->getNextFile().c_str());
        throw std::runtime_error("Data consistency error! Check logs.");
    }

    return true;
}

bool Allocator::getActiveSession(UUID &session_uuid, Session &session) {
    std::lock_guard<std::mutex> lock(allocatorLock);
    if (!isAllocatorEnabled())
        return false;

    if (sessionsActive.find(session_uuid) == sessionsActive.end()) {
        log_info("Active session not found: %s", session_uuid.unparseUpper().c_str());
        return false;
    }

    session = sessionsActive[session_uuid];
    return true;
}

bool Allocator::endSession(UUID &session_uuid) {
    std::lock_guard<std::mutex> lock(allocatorLock);
    if (!isAllocatorEnabled())
        return false;

    log_info("Ending active session: %s", session_uuid.unparseUpper().c_str());

    if (sessionsActive.find(session_uuid) == sessionsActive.end()) {
        log_info("Active session not found: %s", session_uuid.unparseUpper().c_str());
        return false;
    }

    UUID user_uuid = sessionsActive[session_uuid].getUser();
    userSessionsActive[user_uuid]--;
    if (userSessionsActive[user_uuid] == 0)
        userSessionsActive.erase(user_uuid);

    sessionsActive[session_uuid].setSessionState(Session::COMPLETE);
    sessionsActive.erase(session_uuid);

    return true;
}

bool Allocator::expireActiveSession(UUID &session_uuid) {
    std::lock_guard<std::mutex> lock(allocatorLock);
    if (!isAllocatorEnabled())
        return false;

    log_info("Expiring active session: %s", session_uuid.unparseUpper().c_str());
    if (sessionsActive.find(session_uuid) == sessionsActive.end()) {
        log_info("Not active: %s", session_uuid.unparseUpper().c_str());
        return false;
    }

    return swapSession(session_uuid);
}

bool Allocator::expireAllActiveSessions() {
    std::lock_guard<std::mutex> lock(allocatorLock);
    if (!isAllocatorEnabled())
        return false;

    log_info("Expiring all active sessions.");

    std::map<UUID, Session> tmp(sessionsActive);
    for (const std::pair<UUID, Session>& key : tmp) {
        Session s = key.second;
        if (!swapSession(s.getSession()))
            return false;
    }
    return true;
}

bool Allocator::isValidOwner(const UUID &session_uuid, const UUID &user_uuid) {
    std::lock_guard<std::mutex> lock(allocatorLock);
    if (!isAllocatorEnabled())
        return false;
        
    if (sessionsActive.find(session_uuid) == sessionsActive.end())
        return false;
    if (sessionsActive[session_uuid].getUser() != user_uuid)
        return false;
    return true;
}

bool Allocator::isSessionActive(const UUID &session_uuid) const {
    return sessionsActive.find(session_uuid) != sessionsActive.end();
}

uint32_t Allocator::getActiveSessionsForUser(const UUID &user_uuid) {
    if (userSessionsActive.find(user_uuid) == userSessionsActive.end())
        return 0;
    return userSessionsActive[user_uuid];
}

bool Allocator::activateRandomExpiredSession(const UUID &user_uuid, Session &session) {
    if (sessionsExpired.size() == 0)
        return false;

    std::map<UUID, Session>::const_iterator exp_begin = sessionsExpired.cbegin();
    UUID expired_uuid = exp_begin->first;

    sessionsExpired[expired_uuid].setUser(user_uuid);
    session = sessionsExpired[expired_uuid];
    if (!swapSession(expired_uuid))
        return false;
 
    return true;
}

bool Allocator::swapSession(UUID session_uuid) {
    std::map<UUID, Session> *map_from;
    std::map<UUID, Session> *map_to;
    Session::SessionState new_state;
    bool increment = false;

    if (sessionsActive.find(session_uuid) != sessionsActive.end() && 
        sessionsExpired.find(session_uuid) != sessionsExpired.end()) {
        log_error("Yikes! Session UUID %s is active and expired!", session_uuid.unparseUpper().c_str());
        return false;
    } else if (sessionsActive.find(session_uuid) != sessionsActive.end()) {
        log_info("Expiring active session: %s", session_uuid.unparseUpper().c_str());
        map_from = &sessionsActive;
        map_to = &sessionsExpired;
        new_state = Session::SessionState::EXPIRED;
        increment = false;
    } else if (sessionsExpired.find(session_uuid) != sessionsExpired.end()) {
        log_info("Activating expired session: %s", session_uuid.unparseUpper().c_str());
        map_from = &sessionsExpired;
        map_to = &sessionsActive;
        new_state = Session::SessionState::ACTIVE;
        increment = true;
    } else {
        log_info("Swap failure. Session UUID %s not found.", session_uuid.unparseUpper().c_str());
        return false;
    }

    (*map_to)[session_uuid] = (*map_from)[session_uuid];
    (*map_from).erase(session_uuid);
    (*map_to)[session_uuid].setSessionState(new_state);

    UUID user_uuid = (*map_to)[session_uuid].getUser();
    if (increment)
        userSessionsActive[user_uuid]++;
    else
        userSessionsActive[user_uuid]--;
    if (userSessionsActive[user_uuid] == 0)
        userSessionsActive.erase(user_uuid);

    log_info("Swapped session: %s", session_uuid.unparseUpper().c_str());

    return true;
}

bool Allocator::disable() {
    std::lock_guard<std::mutex> lock(allocatorLock);
    log_info("Disabling allocator.");
    allocatorEnabled = false;
    return true;
}

bool Allocator::enable() {
    std::lock_guard<std::mutex> lock(allocatorLock);
    log_info("Enabling allocator.");
    allocatorEnabled = true;
    return true;
}

bool Allocator::isAllocatorEnabled() const {
    if (!allocatorEnabled) {
        log_info("Allocator has been disabled.");
        return false;
    }
    return true;
}

void Allocator::init() {
    log_info("Initializing allocator...");

    std::lock_guard<std::mutex> lock(allocatorLock);
    allocatorEnabled = false;
    if (!initNext() || ! initSessions())
        throw std::runtime_error("Allocator failed to initialize! Check logs.");

    log_info(" Active Assignments: %u", sessionsActive.size());
    log_info("Expired Assignments: %u", sessionsExpired.size());
}

bool Allocator::initNext() {
    log_info("Parsing next assignment...");

    if (serverConfig->getNextFile().size() == 0) {
        log_error("Next assignment file name not set.");
        return false;
    }

    nextRootValue = json_parse_file(serverConfig->getNextFile().c_str());
    if (nextRootValue == NULL) {
        log_error("Next assignments file is missing or invalid: %s", serverConfig->getNextFile().c_str());
        return false;
    }

    nextRootObject = json_value_get_object(nextRootValue);
    if (nextRootObject == NULL) {
        log_error("Next assignments file is invalid: %s", serverConfig->getNextFile().c_str());
        return false;
    }

    const char* alg = json_object_get_string(nextRootObject, "next_start_alg");
    if (alg == NULL) {
        log_error("Next assignments file is missing next_start_alg.");
        return false;
    }
    nextStartAlg = new Algorithm(alg);

    if (json_object_has_value_of_type(nextRootObject, "session_size", JSONNumber) != 1) {
        log_error("Next assignment file is missing session_size.");
        return false;
    }
    sessionSize = (unsigned int)json_object_get_number(nextRootObject, "session_size");

    return true;
}

bool Allocator::initSessions() {
    log_info("Compiling session list...");

    for (const std::filesystem::directory_entry &d: 
            std::filesystem::directory_iterator(serverConfig->getIncompleteSessionsDir())) {
        if (!d.is_symlink())
            continue;

        UUID s_uuid(d.path().filename());
        if (s_uuid.isNull()) {
            log_info("Found invalid session UUID %s", d.path().filename().c_str());
            continue;
        }

        Session s;
        s.setConfig(serverConfig);
        if (!s.readSession(s_uuid))
            continue;
        sessionsActive[s.getUser()] = s;
        userSessionsActive[s.getUser()]++;

// TODO - Update checkpointLast for the session in order to give everyone a fair shot at not expiring
    }

    return true;
}
