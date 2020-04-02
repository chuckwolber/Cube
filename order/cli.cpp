/**
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2019 Chuck Wolber
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

#include <cstring>
#include <getopt.h>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include "../Cube.h"
#include "../Algorithm.h"

std::mutex coutMutex;

std::string findOrders;
std::vector<bool> foundOrders;
unsigned int skip_nth;
unsigned int numThreads;
unsigned int algorithmCountMax;
unsigned int numSkipFoundOrders;
unsigned int heartbeat;
unsigned int foundOrder;
bool keepDuplicates;
bool skipFoundOrders;
bool showFoundOrder;
Algorithm initialAlgorithm;

const int DEFAULT_ALG_MAX = 1000000;
const int ORDER_MAX = 1261;

static struct option longopts[] = {
    {"algstart",    required_argument, nullptr, 'a'},
    {"algmath-add", required_argument, nullptr, 'p'},
    {"algmath-lt",  required_argument, nullptr, 'l'},
    {"count",       required_argument, nullptr, 'c'},
    {"heartbeat",   required_argument, nullptr, 'b'},
    {"keep-dupes",  no_argument,       nullptr, 'k'},
    {"skip-nth",    required_argument, nullptr, 's'},
    {"threads",     required_argument, nullptr, 't'},
    {"find-orders", required_argument, nullptr, 'f'},
    {"find-order",  required_argument, nullptr, 'o'},
    {"help",        optional_argument, nullptr, 'h'},
    { NULL,         0,                 NULL,     0 }
};

void setFindOrders(char* findOrders);
void usage(char* progName);
void calculateOrder(const unsigned int threadNum);
void printResult(const unsigned int threadNum, const unsigned int algNum, const std::vector<Turn> &alg, const unsigned int order);

int main(int argc, char *argv[]) {
    int ch;
    char* algorithmStart = nullptr;
    unsigned int algmathAddVal = 0;
    char* algmathLtVal = nullptr;
    bool algmathAdd = false;
    bool algmathLt = false;
    algorithmCountMax = DEFAULT_ALG_MAX;
    keepDuplicates = false;
    skipFoundOrders = false;
    showFoundOrder = false;
    skip_nth = 1;
    heartbeat = 0;
    numThreads = std::thread::hardware_concurrency();

    opterr = 0;
    while((ch = getopt_long(argc, argv, "a:p:l:c:b:ks:t:f:h", longopts, NULL)) != -1) {
        switch(ch) {
            case 'a':
                algorithmStart = optarg;
                break;
            case 'p':
                algmathAddVal = (unsigned int)(std::stoul(optarg, nullptr, 10));
                algmathAdd = true;
                break;
            case 'l':
                algmathLtVal = optarg;
                algmathLt = true;
                break;
            case 'c':
                algorithmCountMax = (unsigned int)(std::stoul(optarg, nullptr, 10));
                break;
            case 'b':
                heartbeat = (unsigned int)(std::stoul(optarg, nullptr, 10));
                break;
            case 'k':
                keepDuplicates = true;
                break;
            case 's':
                skip_nth = (unsigned int)std::stoi(optarg, nullptr, 10);
                ++skip_nth;
                break;
            case 't':
                numThreads = (unsigned int)std::stoul(optarg, nullptr, 10);
                break;
            case 'f':
                setFindOrders(optarg);
                break;
            case 'o':
                showFoundOrder = true;
                foundOrder = (unsigned int)std::stoul(optarg, nullptr, 10);
                break;
            case 'h':
            default:
                usage(argv[0]);
                return 0;
                break;
        }
    }

    if (skip_nth == 0)
        skip_nth = 1;
    if (numThreads < 1)
        numThreads = 1;
    std::vector<std::thread> threads(numThreads);

    initialAlgorithm.reset();
    if (algorithmStart != nullptr)
        initialAlgorithm.setAlgorithm(algorithmStart);

    if (algmathAdd) {
        Algorithm endAlgorithm(initialAlgorithm);
        endAlgorithm += algmathAddVal;
        std::cout << endAlgorithm.getAlgorithmStr() << std::endl;
        return 0;
    } else if (algmathLt) {
        Algorithm ltAlgorithm(algmathLtVal);
        if (initialAlgorithm < ltAlgorithm)
            std::cout << "true" << std::endl;
        else
            std::cout << "false" << std::endl;
        return 0;
    }

    std::cerr << "Algorithm Count: " << algorithmCountMax << std::endl;
    std::cerr << "Algorithm Start: " << initialAlgorithm.getAlgorithmStr() << std::endl;

    if (skipFoundOrders)
        std::cerr << "Finding Orders: " << findOrders << std::endl;
    std::cerr << "Threads: " << numThreads << std::endl;
   
    for (unsigned int i=0; i<numThreads; i++)
        threads.at(i) = std::thread(calculateOrder, i);
    for (std::thread &t : threads)
        t.join();

    if (heartbeat > 0)
        std::cout << "HB:-1" << std::endl;

    return 0;
}

void setFindOrders(char* orderList) {
    std::string temp = "";
    skipFoundOrders = true;
    numSkipFoundOrders = 0;

    bool foundDefault = true;
    if (strlen(orderList) == 0) {
        foundDefault = false;
        numSkipFoundOrders = ORDER_MAX;
    }
    for (int i = 0; i < ORDER_MAX; i++)
        foundOrders.push_back(foundDefault);

    unsigned int currOrder = 0;
    for (char c : std::string(orderList)) {
        temp += c;
        if (c == ',' && currOrder > 0 && currOrder < ORDER_MAX) {
            foundOrders[currOrder] = false;
            currOrder = 0;
            ++numSkipFoundOrders;
            findOrders += temp;
            temp = "";
        } else if (c == ',') {
            currOrder = 0;
            temp = "";
        } else if (c >= '0' && c <= '9') {
            currOrder = currOrder*10 + (unsigned int)(c - '0');
        } else {
            temp.pop_back();
        }
    }

    if (currOrder < ORDER_MAX) {
        foundOrders[currOrder] = false;
        ++numSkipFoundOrders;
        findOrders += temp;
    }

    if (findOrders.back() == ',')
        findOrders.pop_back();

    if (findOrders == "")
        findOrders = "1 - " + std::to_string(ORDER_MAX - 1);
}

void usage(char* progName) {
    std::cerr << "usage: " << progName << " "
              << "[--algstart | -a] "
              << "[--algmath-add | -p] "
              << "[--count | -c] "
              << "[--heartbeat | -b] "
              << "[--keep-dupes | -k] "
              << "[--skip-nth | -s] "
              << "[--threads | -t] "
              << "[--find-orders | -f] "
              << "[--find-order | -o] "
              << "[--help | -h] "
              << std::endl;
    std::cerr << std::endl;
    std::cerr << " [--algstart | -a]     - The algorithm to start with. Default is "
              << "\"F\". Base-12 counting" << std::endl;
    std::cerr << "                         order is F F' U U' R R' D D' L L' B B'"
              << std::endl;
    std::cerr << " [--algmath-add | -p ] - Add arg value to --algstart to get alg end."
              << std::endl;
    std::cerr << " [--algmath-lt | -p ]  - Returns true if --algstart is less than "
              << "arg value, false otherwise." << std::endl;
    std::cerr << " [--count | -c]        - The number of algorithms to calculate. "
              << "Input is interpreted" << std::endl
              << "                         as an unsigned integer. Default is "
              << "1,000,000." << std::endl;
    std::cerr << " [--heartbeat | -b]    - Display a heartbeat during --find-orders,"
              << " equivalent to every" << std::endl;
    std::cerr << "                         arg attempts." << std::endl;
    std::cerr << " [--keep-dupes | -k]   - Keep algorithms that contain duplication."
              << std::endl;
    std::cerr << " [--skip-nth | -s]     - Skip nth algorithm." << std::endl;
    std::cerr << " [--threads | -t]      - The number of threads to deploy. Default "
              << "is one per CPU." << std::endl;
    std::cerr << " [--find-orders | -f]  - Only find orders that are in this list. "
              << "Empty list \"\" to"
              << std::endl;
    std::cerr << "                         find all orders."
              << std::endl;
    std::cerr << " [--find-order]        - Find all algorithms matching the optarg order."
              << std::endl;
    std::cerr << " [--help | -h]         - Display this messages."
              << std::endl;
    std::cerr << std::endl;
}

void calculateOrder(const unsigned int threadNum) {
    Algorithm algorithm(initialAlgorithm);
    std::vector<Turn> turnSet;
    Cube c(CubieColor::RED, 3);
    unsigned int order;
    unsigned int algorithmCount = threadNum;

    algorithm += threadNum;
    while (algorithmCount < algorithmCountMax) {
        if (algorithmCount % skip_nth != 0)
            goto bypass;
        if (keepDuplicates || (!algorithm.hasInversion() && !algorithm.hasTriple())) {
            order = 0;
            turnSet = algorithm.getAlgorithm();
            do {
                ++order;
                c.performAlgorithm(turnSet);
            } while (!c.isSolved());

            printResult(threadNum, algorithmCount, turnSet, order);
            if (skipFoundOrders && !numSkipFoundOrders)
                return;
        }
        
        bypass:
        algorithmCount += numThreads;
        algorithm += numThreads;

        if (heartbeat > 0 && algorithmCount % heartbeat == 0) {
            coutMutex.lock();
            std::cout << "HB:" << algorithmCount << std::endl;
            coutMutex.unlock();
        }
    }
}

/* The cli_aggregate utility depends on this output format. */
void printResult(const unsigned int threadNum, const unsigned int algNum, const std::vector<Turn> &alg, const unsigned int order) {
    coutMutex.lock();

    if (skipFoundOrders) {
        if (foundOrders[order]) {
            coutMutex.unlock();
            return;
        }
        foundOrders[order] = true;
        --numSkipFoundOrders;
    } else if (showFoundOrder && order != foundOrder) {
        coutMutex.unlock();
        return;
    }

    std::cout << "TN:" << threadNum;
    std::cout << "\tAN:" << algNum;
    std::cout << "\tOR:" << order;
    std::cout << "\tAG:";
    for (const Turn &t : alg)
        std::cout << Algorithm::layerToChar(t.layer)
                  << (t.clockwise ? " ": "' ");
    std::cout << std::endl;
    coutMutex.unlock();
}
