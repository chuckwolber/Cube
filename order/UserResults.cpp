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

#include <stdexcept>

#include "../Algorithm.h"
#include "log.h"
#include "UserResults.hpp"

UserResults::UserResults(const ServerConfig* serverConfig, UUID user) {
    jResultsRootValue = NULL;

    this->serverConfig = serverConfig;
    this->user = user;
    this->resultsFile = serverConfig->getResultsFile(this->user);
}

UserResults::~UserResults() {
    if (jResultsRootValue != NULL)
        json_value_free(jResultsRootValue);
}

std::vector<DiscoveredOrder> UserResults::getResults() {
    return results;
}

void UserResults::insertResult(DiscoveredOrder result) {
    results.push_back(result);

    if (jResultsRootValue == NULL) {
        jResultsRootValue = json_value_init_object();
        jResultsRootObject = json_value_get_object(jResultsRootValue);
        json_object_set_string(jResultsRootObject, "user", user.unparseUpper().c_str());

        JSON_Value* rVal = json_value_init_array();
        json_object_set_value(json_value_get_object(jResultsRootValue), "results", rVal);
        jResultsArray = json_object_get_array(jResultsRootObject, "results");
    }
    
    JSON_Value* resultVal = json_value_init_object();
    JSON_Object* resultObj = json_value_get_object(resultVal);
    json_object_set_string(resultObj, "algorithm", result.algorithm.c_str()); 
    json_object_set_number(resultObj, "order", result.order);
    json_object_set_string(resultObj, "session", result.session.unparseUpper().c_str());
    json_array_append_value(jResultsArray, resultVal);
    
    if (JSONFailure == json_serialize_to_file_pretty(jResultsRootValue, resultsFile.c_str())) {
        log_error("Failed to serialize JSON: %s", resultsFile.c_str());
        throw std::runtime_error("Data consistency error! Check logs.");
    }
}

bool UserResults::readUserResults() {
    if (!parseRootValue()  ||
        !parseRootObject() ||
        !parseUser()       ||
        !parseResults())
        return false;
    return true;
}

bool UserResults::parseRootValue() {
    log_info("Parsing UserResults JSON file: %s", resultsFile.c_str());
    jResultsRootValue = json_parse_file(resultsFile.c_str());
    if (jResultsRootValue == NULL) {
        log_error("UserResults file missing or invalid: %s", resultsFile.c_str());
        return false;
    }

    return true;
}

bool UserResults::parseRootObject() {
    jResultsRootObject = json_value_get_object(jResultsRootValue);
    if (jResultsRootObject == NULL) {
        log_error("UserResults file invalid: %s", resultsFile.c_str());
        return false;
    }

    return true;
}

bool UserResults::parseUser() {
    const char* user_uuid_str = json_object_get_string(jResultsRootObject, "user");
    if (user_uuid_str == NULL) {
        log_error("UserResults %s is missing user UUID.", resultsFile.c_str());
        return false;
    }
    UUID user_uuid(user_uuid_str);
    if (user_uuid.isNull()) {
        log_error("UserResults %s has an invalid user UUID: %s", resultsFile.c_str(), user_uuid_str);
        return false;
    }
    user = user_uuid;

    return true;
}

bool UserResults::parseResults() {
    jResultsArray = json_object_get_array(jResultsRootObject, "results");
    if (jResultsArray == NULL) {
        log_error("Invalid results array in %s", resultsFile.c_str());
        return false;
    }

    size_t numResults = json_array_get_count(jResultsArray);
    for (size_t i = 0; i < numResults; i++) {
        DiscoveredOrder r;
        JSON_Object* jR = json_array_get_object(jResultsArray, i);

        UUID s_uuid(json_object_get_string(jR, "session"));
        r.algorithm = json_object_get_string(jR, "algorithm");
        r.order = (unsigned int)json_object_get_number(jR, "order");
        r.session = s_uuid;

        if (r.algorithm.size() == 0 || !Algorithm::isValid(r.algorithm.c_str())) { 
            log_error("Results json entry %d invalid algorithm %s", i, r.algorithm.c_str());
            return false;
        }

        if (r.order == 0 || r.session.isNull()) {
            log_error("Results json entry %d is invalid.", i);
            return false;
        }

        results.push_back(r);
    }

    return true;
}
