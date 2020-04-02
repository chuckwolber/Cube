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
#include <experimental/filesystem>

#include "../Algorithm.h"
#include "Session.hpp"
#include "log.h"

// TODO - This object also owns checkpoint log.
// TODO - Add results to a session.
// TODO - Add dateTime elements to session.
// TODO - Add a checkpoint log entry when a session is created.
//        Because that involves other meta-info, it might be best left up to a secondary call.
// TODO - Update readSession and newSession for every session variable.

Session::Session() {}

Session::Session(const Session& obj) {
    serverConfig = obj.serverConfig;
    user = obj.user;
    session = obj.session;
    algorithm = obj.algorithm;
    size = obj.size;
    state = obj.state;
}

Session& Session::operator=(const Session& rhs) {
    if (this != &rhs) {
        serverConfig = rhs.serverConfig;
        user = rhs.user;
        session = rhs.session;
        algorithm = rhs.algorithm;
        size = rhs.size;
        state = rhs.state;
    }
    return *this;
}

bool Session::operator==(const Session& rhs) const {
    if (user != rhs.user)
        return false;
    if (session != rhs.session)
        return false;
    if (algorithm != rhs.algorithm)
        return false;
    if (size != rhs.size)
        return false;
    if (state != rhs.state)
        return false;
    return true;
}

void Session::setConfig(const ServerConfig* serverConfig) {
    this->serverConfig = serverConfig;
}

bool Session::readSession(UUID session) {
    log_info("Reading session: %s", session.unparseUpper().c_str());

    if (serverConfig == NULL) {
        log_error("Server config not set.");
        return false;
    }

    if (sessionOwned) {
        log_error("Session is already owned.");
        return false;
    }
    sessionOwned = true;

    this->session = session;
    if (!parseSessionRootValue()  ||
        !parseSessionRootObject() ||
        !parseUser()              ||
        !parseSession()           ||
        !parseSessionState()      ||
        !parseAlgorithm()         ||
        !parseSize())
        return false;

    if (!isSessionValid())
        return false;
    return true;
}

bool Session::newSession(UUID user, std::string algorithm, unsigned int size) {
    log_info("New session (%s) for user (%s).",
            session.unparseUpper().c_str(), user.unparseUpper().c_str());

    if (serverConfig == NULL) {
        log_error("Server config not set.");
        return false;
    }

    if (sessionOwned) {
        log_error("Session is already owned.");
        return false;
    }
    sessionOwned = true;

    if (!isSessionNew(user)) {
        log_error("Session is not new!");
        throw std::runtime_error("Data consistency error! Check logs.");
    }

    sessionRootValue = json_value_init_object();
    sessionRootObject = json_value_get_object(sessionRootValue);
    this->user = user;
    this->state = Session::ACTIVE;
    this->algorithm = algorithm;
    this->size = size;

    if (!persistSession())
        return false;
    
    if (!createSessionsLink(user) || !createIncompleteSessionsLink(user))
        throw std::runtime_error("Data consistency error! Check logs.");

    return true;
}

bool Session::setUser(UUID user) {
    if (serverConfig == NULL) {
        log_error("Server config not set.");
        return false;
    }
    if (!sessionOwned) {
        log_error("Session is unowned.");
        return false;
    }
    if (this->user == user)
        return true;

    this->user = user;
    if (!persistSession())
        return false;
    
    // TODO - When a session is re-owned, leave a checkpoint entry in previous owner's log.
    // New owner's checkpoint log has entry to previous owner's UUID.
    // A session can be re-owned back to the original owner.

    return true;
}

bool Session::setSessionState(Session::SessionState state) {
    if (serverConfig == NULL) {
        log_error("Server config not set.");
        return false;
    }
    if (!sessionOwned) {
        log_error("Session is unowned.");
        return false;
    }
    if (state == SessionState::UNKNOWN) {
        log_error("Setting state to UNKNOWN is invalid.");
        return false;
    }
    if (this->state == state)
        return true;

    this->state = state;
    if (!persistSession())
        return false;

    // TODO - If state is complete, remove incompleteSessions link.
    // Otherwise create incompleteSessions link (allows us to resurrect a completed session).

    return true;
}

UUID Session::getUser() {
    return user;
}

UUID Session::getSession() {
    return session;
}

std::string Session::getAlgorithm() {
    return algorithm;
}

unsigned int Session::getSize() {
    return size;
}

Session::SessionState Session::getSessionState() {
    return state;
}

std::string Session::sessionStateToString(SessionState state) {
    switch(state) {
        case ACTIVE: {
            return std::string("ACTIVE");
            break;
        }
        case EXPIRED: {
            return std::string("EXPIRED");
            break;
        }
        case COMPLETE: {
            return std::string("COMPLETE");
            break;
        }
        default:
            return std::string("UNKNOWN");
    }
}

Session::SessionState Session::stringToSessionState(std::string state) {
    if (state == "ACTIVE")
        return SessionState::ACTIVE;
    if (state == "EXPIRED")
        return SessionState::EXPIRED;
    if (state == "COMPLETE")
        return SessionState::COMPLETE;

    return SessionState::UNKNOWN;
}

bool Session::isSessionValid() {
    if (!sessionFileExists(user)) {
        log_info("Session does not exist: %s", serverConfig->getSessionFile(user, session).c_str());
        return false;
    }

    if (!Algorithm::isValid(algorithm.c_str())) {
        log_error("Invalid algorithm: %s", algorithm.c_str());
        return false;
    }

    if (state == UNKNOWN) {
        log_error("Invalid state - UNKNOWN");
        return false;
    }

    if (size == 0) {
        log_error("Session size of 0 is invalid.");
        return false;
    }

    return true;
}

bool Session::isSessionNew(UUID user) {
    if (sessionFileExists(user)) {
        log_info("Session file exists: %s", serverConfig->getSessionFile(user, session).c_str());
        return false;
    }

    if (sessionsLinkExists()) {
        log_info("Sessions link exists: %s", serverConfig->getSessionFile(session).c_str());
        return false;
    }

    return true;
}

bool Session::sessionFileExists(UUID user) {
    return std::filesystem::is_regular_file(serverConfig->getSessionFile(user, session));
}

bool Session::sessionsLinkExists() {
    return std::filesystem::is_symlink(serverConfig->getSessionFile(session));
}

bool Session::createSessionsLink(UUID user) {
    std::error_code ec;

    std::string target = serverConfig->getSessionDir(user, session);
    std::string link = serverConfig->getSessionDir(session);
    std::experimental::filesystem::remove(link, ec);

    ec.clear();
    std::experimental::filesystem::create_directory_symlink(target, link, ec);

    if (ec.value() != 0) {
        log_error("Link creation error (%d): %s", ec.value(), ec.message().c_str());
        return false;
    }
    return true;
}

bool Session::createIncompleteSessionsLink(UUID user) {
    std::error_code ec;

    std::string target = serverConfig->getSessionDir(user, session);
    std::string link = serverConfig->getIncompleteSessionDir(session);
    std::experimental::filesystem::remove(link, ec);

    ec.clear();
    std::experimental::filesystem::create_directory_symlink(target, link, ec);

    if (ec.value() != 0) {
        log_error("Link creation error (%d): %s", ec.value(), ec.message().c_str());
        return false;
    }
    return true;
}

bool Session::persistSession() {
    if (serverConfig == NULL) {
        log_error("Server config not set.");
        return false;
    }

    json_object_set_string(sessionRootObject, "user", user.unparseUpper().c_str());
    json_object_set_string(sessionRootObject, "session", session.unparseUpper().c_str());
    json_object_set_string(sessionRootObject, "state", Session::sessionStateToString(state).c_str());
    json_object_set_string(sessionRootObject, "algorithm", algorithm.c_str());
    json_object_set_number(sessionRootObject, "session_size", size);

    if (JSONFailure == json_serialize_to_file_pretty(sessionRootValue, serverConfig->getSessionFile(user, session).c_str())) {
        log_error("Failed to serialize JSON: %s", serverConfig->getSessionFile(user, session).c_str());
        throw std::runtime_error("Data consistency error! Check logs.");
    }

    return true;
}

bool Session::parseSessionRootValue() {
    log_info("Parsing session JSON file: %s", serverConfig->getSessionFile(session).c_str());
    sessionRootValue = json_parse_file(serverConfig->getSessionFile(session).c_str());
    if (sessionRootValue == NULL) {
        log_error("Session file missing or invalid: %s", serverConfig->getSessionFile(session).c_str());
        return false;
    }

    return true;
}

bool Session::parseSessionRootObject() {
    sessionRootObject = json_value_get_object(sessionRootValue);
    if (sessionRootObject == NULL) {
        log_error("Session file invalid: %s", serverConfig->getSessionFile(session).c_str());
        return false;
    }

    return true;
}

bool Session::parseUser() {
    const char* user_uuid_str = json_object_get_string(sessionRootObject, "user");
    if (user_uuid_str == NULL) {
        log_error("Session %s is missing user UUID.", session.unparseUpper().c_str());
        return false;
    }
    UUID user_uuid(user_uuid_str);
    if (user_uuid.isNull()) {
        log_error("Session %s has an invalid user UUID: %s", session.unparseUpper().c_str(), user_uuid_str);
        return false;
    }
    user = user_uuid;

    return true;
}

bool Session::parseSession() {
    const char* session_uuid_str = json_object_get_string(sessionRootObject, "session");
    if (session_uuid_str == NULL) {
        log_error("Session %s is missing session UUID.", session.unparseUpper().c_str());
        return false;
    }
    UUID session_uuid(session_uuid_str);
    if (session_uuid.isNull()) {
        log_error("Session %s has invalid session UUID: %s", session.unparseUpper().c_str(), session_uuid_str);
        return false;
    }
    if (session_uuid != session) {
        log_error("Session %s has UUID %s set internally.",
                session.unparseUpper().c_str(), session_uuid.unparseUpper().c_str());
        return false;
    }

    return true;
}

bool Session::parseSessionState() {
    const char* session_state_str = json_object_get_string(sessionRootObject, "session_state");
    if (session_state_str == NULL) {
        log_error("Session %s is missing session_state.", session.unparseUpper().c_str());
        return false;
    }
    state = stringToSessionState(session_state_str);
    if (state == SessionState::UNKNOWN) {
        log_error("Session %s has an invalid session_state: %s",
                    session.unparseUpper().c_str(), session_state_str);
        return false;
    }

    return true;
}

bool Session::parseAlgorithm() {
    const char* algorithm_str = json_object_get_string(sessionRootObject, "algorithm");
    if (algorithm_str == NULL) {
        log_error("Session %s is missing algorithm.", session.unparseUpper().c_str());
        return false;
    }
    algorithm = algorithm_str;

    return true;
}

bool Session::parseSize() {
    if (json_object_has_value_of_type(sessionRootObject, "session_size", JSONNumber) != 1) {
        log_error("Session %s is missing session_size.", session.unparseUpper().c_str());
        return false;
    }
    size = (unsigned int)json_object_get_number(sessionRootObject, "session_size");

    return true;
}
