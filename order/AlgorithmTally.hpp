/**
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2022 Chuck Wolber
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

#include <vector>

#include "../Algorithm.hpp"
#include "SchwartzGeneratorReduce.hpp"

#ifndef ALGORITHMTALLY_H
#define ALGORITHMTALLY_H

typedef bool (Algorithm::*RedundancyEvaluator)();
typedef std::vector<unsigned long long int> AlgorithmList;

class AlgorithmTally {
    public:
        Algorithm* algorithm = new Algorithm();
        AlgorithmList* algorithms = new AlgorithmList();

        ~AlgorithmTally() {
            delete algorithm;
            delete algorithms;
        }
};

class Algorithms : public SchwartzGeneratorReduce<AlgorithmTally, AlgorithmList> {
    public:
        Algorithms(size_t n_threads, unsigned long long int dataSize, RedundancyEvaluator re) : 
            SchwartzGeneratorReduce<AlgorithmTally, AlgorithmList>(n_threads, dataSize) {
            this->re = re;
        } 

    private:
        RedundancyEvaluator re;

    protected:
        virtual AlgorithmTally* init() const {
            return new AlgorithmTally();
        }

        virtual AlgorithmTally* combine(const AlgorithmTally* left, const AlgorithmTally* right) const {
            AlgorithmTally* at = init();
            at->algorithms->insert(at->algorithms->end(), left->algorithms->begin(), left->algorithms->end());
            at->algorithms->insert(at->algorithms->end(), right->algorithms->begin(), right->algorithms->end());
            return at;
        }

        virtual AlgorithmList* gen(AlgorithmTally* tally) {
            return tally->algorithms;
        }

        virtual void accum(AlgorithmTally* accumulator, unsigned long long int index) const {
            unsigned long long int x = accumulator->algorithm->getAlgorithmNumber();
            *(accumulator->algorithm) += (index - x);
            if (!(*(accumulator->algorithm).*re)())
                accumulator->algorithms->push_back(index);
        }
};

#endif // ALGORITHMTALLY_H