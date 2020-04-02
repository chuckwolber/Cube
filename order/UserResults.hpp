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

#ifndef RESULT_HPP
#define RESULT_HPP

#include <vector>

#include "log.h"
#include "parson.h"
#include "ServerConfig.hpp"
#include "UUID.hpp"

struct DiscoveredOrder {
    std::string algorithm;
    UUID session;
    unsigned int order;
};

class UserResults {
    public:
        UserResults(const ServerConfig* serverConfig, UUID user);
        ~UserResults();

        std::vector<DiscoveredOrder> getResults();
        void insertResult(DiscoveredOrder result);
        bool readUserResults();
    private:
        const ServerConfig* serverConfig;
        std::string resultsFile;
        UUID user;

        std::vector<DiscoveredOrder> results;
        JSON_Value* jResultsRootValue;
        JSON_Object* jResultsRootObject;
        JSON_Array* jResultsArray;

        bool parseRootValue();
        bool parseRootObject();
        bool parseUser();
        bool parseResults();

        void parseResultsJson();

        const std::string resultsSchema =            \
        "{                                           \
            \"user\": \"${USER}\",                   \
            \"results\": [                           \
                {                                    \
                    \"algorithm\": \"${ALGORITHM}\", \
                    \"order\": ${ORDER},             \
                    \"session\": \"${SESSION}\"      \
                }                                    \
            ]                                        \
        }";
};

#endif // RESULT_HPP
