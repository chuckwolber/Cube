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

#include "log.h"
#include "Results.hpp"

/* Client Only */
Results::Results() {
    initResults();
}

/* Server Only */
Results::Results(const ServerConfig* serverConfig) {
    this->serverConfig = serverConfig;
    initResults();
    loadResults();
    updateMissingOrders();
}

Results::~Results() {
    for (const std::pair<UUID, UserResults*>& result : userResults)
        delete result.second;
}

/* Client Only */
bool Results::isOrderNovel(unsigned int order) {
    return !(results[order].order > 0);
}

/* Client Only */
void Results::setMissingOrders(std::string missingOrders) {
    std::string orderNum = "";
    std::vector<unsigned int> missing(RESULTS_MAX + 1);

    for (unsigned int i = 0; i < missingOrders.length(); i++) {
        if (std::isdigit(missingOrders[i])) {
            orderNum += missingOrders[i];
        } else if (missingOrders[i] == ',') {
            unsigned int order = (unsigned int)stoul(orderNum);
            missing[order] = order;
            orderNum = "";
        }
    }
    
    if (orderNum.length() != 0) {
        unsigned int order = (unsigned int)stoul(orderNum);
        missing[order] = order;
    }

    for (unsigned int i = 1; i <= RESULTS_MAX; i++)
        if (missing[i] == 0)
            results[i].order = i;
}

/* Server Only */
bool Results::insertResult(DiscoveredOrder result, UUID user) {
    std::lock_guard<std::mutex> lock(resultsLock);
    if (results[result.order].order > 0)
        return true;

    if (userResults.find(user) == userResults.end()) {
        userResults[user] = new UserResults(serverConfig, user);

        std::error_code ec;
        std::string target = serverConfig->getResultsFile(user);
        std::string link = serverConfig->getResultsLink(user);
        std::experimental::filesystem::remove(link, ec);

        ec.clear();
        std::experimental::filesystem::create_symlink(target, link, ec);
        if (ec.value() != 0) {
            log_error("Link creation error (%d): %s", ec.value(), ec.message().c_str());
            return false;
        }
    }

    userResults[user]->insertResult(result);
    results[result.order] = result;
    updateMissingOrders();

    return true;
}

/* Server Only */
std::string Results::getMissingOrders() {
    std::lock_guard<std::mutex> lock(resultsLock);
    return missingOrders;
}

void Results::initResults() {
    DiscoveredOrder no;
    no.order = 0;

    results.resize(RESULTS_MAX + 1);
    for (unsigned int i = 0; i <= RESULTS_MAX; i++)
        results.push_back(no);
}

/* Server Only */
void Results::loadResults() {
    for (const std::filesystem::directory_entry &d: 
            std::filesystem::directory_iterator(serverConfig->getResultsDir())) {
        if (!d.is_symlink())
            continue;

        UUID u_uuid(d.path().filename());
        if (u_uuid.isNull()) {
            log_info("Found invalid results user UUID %s", d.path().filename().c_str());
            continue;
        }

        UserResults* uResults = new UserResults(serverConfig, u_uuid);
        if (!uResults->readUserResults())
            continue;
        userResults[u_uuid] = uResults;
        
        for (DiscoveredOrder res : uResults->getResults())
            results[res.order] = res;
    }
}

/* Server Only */
void Results::updateMissingOrders() {
    missingOrders = "";
    for (unsigned int i = 1; i <= RESULTS_MAX; i++)
        if (results[i].order == 0)
            missingOrders += std::to_string(i) + ",";
    if (missingOrders[missingOrders.size() - 1] == ',')
        missingOrders.erase(missingOrders.size() - 1);
}
