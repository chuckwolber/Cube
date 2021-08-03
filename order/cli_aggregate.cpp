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

/**
 * Description:
 *    Aggregates cli output into individual files representing each numerical
 *    order. Each file combines all of the known combinations that match that
 *    order.
 * 
 *    File format is:
 *       TN:1    AN:6    OR:4    AG:F' F  F'
 * 
 *    TN - Thread Number
 *    AN - Algorithm Number
 *    OR - Order
 *    AG - Algorithm
 * 
 *    Note 1: For large scale processing, JSON would make more sense. The
 *    intent of the cli was to behave like a "calculator" utility for
 *    determining the order of a set of algorithms. For that reason, human
 *    readability was favored over large scale processing capability.
 * 
 *    Note 2: Algorithm Number is not considered an absolute value, as that
 *    would require numbers far larger than architecture primitives allow. 
 *    Algorithm Number is relative to the thread number in a particular data
 *    file. Think of it as a shorthand for determining whether an algorithm is
 *    greater or less than another algorithm, without needing to bother with
 *    remembering which turn comes before another.
 */

#include <getopt.h>
#include <iostream>
#include <fstream>
#include <string>

static struct option longopts[] = {
   {"data",     required_argument, nullptr, 'd'},
   {"out-path", required_argument, nullptr, 'p'},
   {"help",     optional_argument, nullptr, 'h'}
};

void usage(char* progName);
void aggregateOrder(std::string dataFile, std::string outputPath);

int main(int argc, char *argv[]) {
   int ch;
   std::string mode;
   std::string dataFile;
   std::string outputPath;

   opterr = 0;
   while((ch = getopt_long(argc, argv, "d:p:h", longopts, NULL)) != -1) {
      switch(ch) {
         case 'd':
            dataFile = optarg;
            break;
         case 'p':
            outputPath = optarg;
            break;
         case 'h':
         default:
            usage(argv[0]);
            return 0;
            break;
      }
   }

   aggregateOrder(dataFile, outputPath);

   return 0;
}

void usage(char* progName) {
   std::cerr << "usage: " << progName << " "
             << "[--data | -d] "
             << "[--out-path | -p] "
             << "[--help | -h] "
             << std::endl;
   std::cerr << std::endl;
   std::cerr << " [--data | -d]     - The data file to read." << std::endl;
   std::cerr << " [--out-path | -p] - The path to output to." << std::endl;
   std::cerr << " [--help | -h]     - Display this message."  << std::endl;
   std::cerr << std::endl;
}

void aggregateOrder(std::string dataFile, std::string outputPath) {
   std::ifstream inputData;
   char* data = new char[1024];
   int* orderList = new int[1260];

   for (int i = 0; i < 1260; i++)
      orderList[i] = 0;

   std::cout << "Aggregating Order..." << std::endl;
   std::cout << "Data File: " << dataFile << std::endl;
   std::cout << "Output Path: " << outputPath << std::endl;

   inputData.open(dataFile);
   if (inputData.fail()) {
      std::cout << "Failed to open " << dataFile << std::endl;
      return;
   }

   int n = 0;
   while (inputData.good() && inputData.getline(data, 1024)) {
      n++;
      if (n%10000000 == 0)
         std::cout << "Lines: " << n << std::endl;
      
      int i = 0;
      while(data[i] != '\0') {
         if (data[i] == 'O' && data[i+1] == 'R' && data[i+2] == ':') {
            i+= 3;

            int order = 0;
            while (data[i] - '0' >= 0 && data[i] - '0' <= 9)
               order = order * 10 + (int)(data[i++] - '0');
            orderList[order - 1]++;
            break;
         }
         i++;
      }
   }

   std::cout << "Lines Read: " << n << std::endl;
   for (int i = 0; i < 1260; i++)
      if (orderList[i] > 0)
         std::cout << "Order[" << (i + 1) << "]: " << orderList[i] << std::endl;

   inputData.close();
   delete[] data;
   delete[] orderList;
}
