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
 *
 * Significant portions of this class copied/derived from GeneralScanSchwartz.h
 * as provided by Professor Kevin Lundeen from Seattle University.
 */

#include <future>

#ifndef SCHWARTZGENERATORREDUCE_H
#define SCHWARTZGENERATORREDUCE_H

template<typename TallyType, typename ResultType=TallyType>
class SchwartzGeneratorReduce {
    public:
        typedef std::vector<TallyType*> TallyData;
        static const size_t ROOT = 0;

        SchwartzGeneratorReduce(size_t n_threads, unsigned long long int dataSize) : 
                        reduced(false), n_threads(n_threads), dataSize(dataSize) {
            interior = new TallyData(n_threads * 2);
        }

        virtual ~SchwartzGeneratorReduce() {
            if (interior != NULL) {
                for (TallyType* t : *interior)
                    delete t;
                delete interior;
            }
        }

        ResultType* getReduction() {
            reduced = reduced || reduce(ROOT);
            return gen(value(ROOT));
        }

    protected:
        /**
         * Identity element for tally operation. Generally, 
         * combine(init(), prepare(x)) == prepare(x). Typically for summing, the
         * return is 0, for products, 1.
         * @return identity tally element
         */
        virtual TallyType* init() const = 0;

        /**
         * Combine two tallies. Tallies should be commutative,
         * i.e., combine(a,b) == combine(b,a)
         * For summing, this typically returns left + right.
         */
        virtual TallyType* combine(const TallyType* left, const TallyType* right) const = 0;

        /**
         * Convert a tally to a result. If the ResultType and TallyType are the
         * same, typically this returns tally.
         */
        virtual ResultType* gen(TallyType* tally) = 0;

        /**
         * Combine and replace left with result.
         */
        virtual void accum(TallyType* accumulator, unsigned long long int index) const = 0;

    private:
        TallyData* interior;
        bool reduced;
        size_t n_threads;
        unsigned long long int dataSize;

        TallyType* value(size_t i) {
            return interior->at(i);
        }

        bool reduce(size_t i) {
            if (i < n_threads - 1) {
                auto handle = std::async(std::launch::async, &SchwartzGeneratorReduce::reduce, this, right(i));
                reduce(left(i));
                handle.wait();
                interior->at(i) = combine(value(left(i)), value(right(i)));
            } else {
                TallyType* tally = init();
                unsigned long long int start = getStart(i);
                unsigned long long int end = getEnd(start);
                for (unsigned long long int j = start; j < end; ++j)
                    accum(tally, j);
                interior->at(i) = tally;
            }
            return true;
        }

        size_t left(size_t i) {
            return i * 2 + 1;
        }   

        size_t right(size_t i) {
            return left(i) + 1;
        }

        unsigned long long int getSize() {
            unsigned long long int size = dataSize / n_threads;
            if (size*n_threads != size)
                ++size;
            return size;
        }

        unsigned long long int getStart(size_t node) {
            return (node - n_threads + 1)*getSize();
        }

        unsigned long long int getEnd(size_t start) {
            unsigned long long int size = getSize();
            return (start + size) > dataSize ? dataSize : start + size;
        }
};

#endif // SCHWARTZGENERATORREDUCE_H
