/**
 * MIT License
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

#include <getopt.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "../Cube.h"
#include "../Algorithm.h"

std::mutex algorithmMutex;
std::mutex coutMutex;

Algorithm sharedAlgorithm;
unsigned long sharedAlgorithmCount = 0;
unsigned long algorithmCountMax;
const int DEFAULT_ALG_MAX = 1000000;

static struct option longopts[] = {
   {"algstart", required_argument, nullptr, 'a'},
   {"count",    required_argument, nullptr, 'c'},
   {"threads",  required_argument, nullptr, 't'},
   {"help",     optional_argument, nullptr, 'h'}
};

void usage(char* progName);
void calculateOrder(const int threadNum);
void printResult(const int threadNum, const int algNum, const std::vector<Turn> &alg, const int order);

int main(int argc, char *argv[]) {
   int ch;
   char *algorithmStart = nullptr;
   algorithmCountMax = DEFAULT_ALG_MAX;

   std::vector<std::thread> threads;
   unsigned int numThreads = std::thread::hardware_concurrency();

   opterr = 0;
   while((ch = getopt_long(argc, argv, "a:c:t:h", longopts, NULL)) != -1) {
      switch(ch) {
         case 'a':
            algorithmStart = optarg;
            break;
         case 'c':
            algorithmCountMax = std::stoul(optarg, nullptr, 10);
            break;
         case 't':
            numThreads = std::stoul(optarg, nullptr, 10);
            break;
         case 'h':
         default:
            usage(argv[0]);
            return 0;
            break;
      }
   }

   if (numThreads < 1)
      numThreads = 1;

   sharedAlgorithm.clear();
   if (algorithmStart != nullptr)
      sharedAlgorithm.setAlgorithm(algorithmStart);
   else
      sharedAlgorithm.addTurn({Layer::F, true});

   std::cerr << "Threads: " << numThreads << std::endl;
   std::cerr << "Algorithm Count: " << algorithmCountMax << std::endl;
   std::cerr << "Algorithm Start: ";
   for (const Turn &t : sharedAlgorithm.getAlgorithm())
      std::cerr << Cube::layerToChar(t.layer)
                << (t.clockwise ? " ": "'")
                << " "; 
   std::cerr << std::endl;
   
   for (unsigned int i=0; i<numThreads; i++)
      threads.emplace_back([&]{calculateOrder(i);});
   for (std::thread &t : threads)
      t.join();

   return 0;
}

void usage(char* progName) {
   std::cerr << "usage: " << progName << " "
             << "[--algstart | -a] "
             << "[--count | -c] "
             << "[--threads | -t] "
             << "[--help | -h] "
             << std::endl;
   std::cerr << std::endl;
   std::cerr << " [--algstart | -a] - The algorithm to start with. Default is "
             << "\"F\"." << std::endl;
   std::cerr << " [--count | -c]    - The number of algorithms to calculate. "
             << "Input is interpreted as" << std::endl
             << "                     an unsigned long integer. Default is "
             << "1,000,000." << std::endl;
   std::cerr << " [--threads | -t]  - The number of threads to deploy. Default "
             << "is one per CPU core." << std::endl;
   std::cerr << " [--help | -h]     - Display this messages."
             << std::endl;
   std::cerr << std::endl;
}

void calculateOrder(const int threadNum) {
   std::vector<Turn> localAlgorithm;
   Cube c(CubieColor::RED, 3);
   bool runCalc = true;
   unsigned long order, localAlgorithmCount = 0;

   while (runCalc) {
      algorithmMutex.lock();
      localAlgorithmCount = ++sharedAlgorithmCount;

      if (localAlgorithmCount > algorithmCountMax) {
         runCalc = false;
         algorithmMutex.unlock();
         continue;
      }

      if (sharedAlgorithm.hasInversion() || sharedAlgorithm.hasTriple()) {
         sharedAlgorithm++;
         algorithmMutex.unlock();
         continue;
      }

      localAlgorithm = sharedAlgorithm.getAlgorithm();
      ++sharedAlgorithm;
      algorithmMutex.unlock();

      order = 0;
      do {
         ++order;
         c.performAlgorithm(localAlgorithm);
      } while (!c.isSolved());
      printResult(threadNum, localAlgorithmCount, localAlgorithm, order);
   }
}

/* The cubeorder_aggregate utility depends on this output format. */
void printResult(const int threadNum, const int algNum, const std::vector<Turn> &alg, const int order) {
   coutMutex.lock();
   std::cout << "TN:" << threadNum;
   std::cout << "\tAN:" << algNum;
   std::cout << "\tOR:" << order;
   std::cout << "\tAG:";
   for (const Turn &t : alg)
      std::cout << Cube::layerToChar(t.layer)
                << (t.clockwise ? " ": "'")
                << " ";
   std::cout << std::endl;
   coutMutex.unlock();
}
