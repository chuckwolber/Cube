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

#ifndef RESULTS_HPP
#define RESULTS_HPP

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "UserResults.hpp"
#include "ServerConfig.hpp"
#include "UUID.hpp"

class Results {
    public:
        Results();
        Results(const ServerConfig* serverConfig);
        ~Results();

        /* Client Only */
        bool isOrderNovel(unsigned int order);
        void setMissingOrders(std::string missingOrders);

        /* Server Only */
        bool insertResult(DiscoveredOrder result, UUID user);
        std::string getMissingOrders();

    private:
        static const unsigned int RESULTS_MAX = 1260;
        std::mutex resultsLock;
        const ServerConfig* serverConfig;

        std::string missingOrders;
        std::vector<DiscoveredOrder> results;
        std::map<UUID, UserResults*> userResults;

        void initResults();
        void loadResults();
        void updateMissingOrders();
};

#endif // RESULTS_HPP
