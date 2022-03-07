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

/**
 * Description:
 *    This class is not thread safe.
 * 
 *    This class models an algorithm that can be applied to a Rubik's Cube
 *    (3x3x3). All algorithms are based on the QTM (Quarter Turn Metric) and
 *    are limited to clockwise and anticlockwise versions of the F, U, R, D, L,
 *    and B layers. These limitations simplify modeling a Rubik's Cube in
 *    computer memory, without giving up performance.
 * 
 *    While humans find a great deal of benefit from using expanded shorthand
 *    notation (https://www.speedsolving.com/wiki/index.php/NxNxN_Notation),
 *    such notation makes computing algorithms much more computationally
 *    difficult on a Rubik's Cube.
 * 
 *    Patches are welcomed!
 * 
 * Algorithm Representation:
 *    Algorithms are lexicographically ordered, which is conceptually equivalent
 *    to an [Odometer](https://en.wikipedia.org/wiki/Odometer). They are read
 *    from left to right, and incremented from right to left. The Most
 *    Significant Turn (MST) is the left most digit, and the Least Significant
 *    Turn (LST) is the right most digit.
 *
 *    Where algorithms differ from odometers is in the internal class
 *    representation, the numerical base used to signify each symbol, and the
 *    fact that algorithms are [sequences](https://en.wikipedia.org/wiki/Sequence).
 *
 *    Internally, the algorithm is stored in a vector with the LST at the
 *    lowest index value and the MST at the higest index value. This makes it
 *    quite performant to increment algorithms; index values can be added and
 *    removed from the end of a Vector in constant time (O(1)).
 *
 *    Because there are 12 fundamental turns, each field in an algorithm is
 *    treated as if it is a Base-12 number.
 *
 *    When a odometer rolls over to take up another significant digit (e.g. 99
 *    to 100, or 999 to 1000, etc.), the new significant digit is the second
 *    value in the number system being used; e.g. "1" in the decimal system.
 *    When a sequence rolls over, the new significant digit starts over at the
 *    lowest value in the number system being used; e.g. "0". This enables an
 *    arbitrarily long sequence of the same value.
 *
 *    An odometer can be thought of as a sequence generator if one ignores the
 *    most significant (left most) digit.
 *
 * Glossary of Terms:
 *    * Layer: See glossary in Cube.h for Layer definition.
 *    * LST: Least Significant Turn. The turn on the right side of the human
 *      readable algorithm, and the lowest index value in the Algorithm class'
 *      internal representation.
 *    * MST: Most Significant Turn. The turn on the left side of the human
 *      readable algorithm, and the highest index value in the Algorithm class'
 *      internal representation.
 *    * QTM (Quarter Turn Metric): One turn of one face ninety degrees. This is
 *      contrasted with the HTM (Half Turn Metric), where half turns make up
 *      two QTM turns.
 *    * Turn: The movement of a layer on a Rubik's Cube. A layer can be turned
 *      clockwise or anticlockwise.
 */

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>
#include <vector>

enum Layer {
    U=1, L=4, F=5,
    R=6, B=7, D=9,
    M,   E,   S,  // Middle layers are currently unsupported.
    NOLAYER
};

struct Turn {
    Layer layer;
    bool clockwise;
};

static const Turn initialTurn = {Layer::F, true};

class Algorithm {
    public:
        Algorithm();
        Algorithm(const unsigned long long int algNum);
        Algorithm(const std::vector<Turn> turns);
        Algorithm(const Algorithm& obj);
        Algorithm(const char* algorithm);

        Algorithm& operator= (const Algorithm& rhs);
        Algorithm& operator++();    // prefix increment
        Algorithm  operator++(int); // postfix increment
        Algorithm& operator+=(unsigned long long int x);
        bool       operator==(const Algorithm& rhs);
        bool       operator!=(const Algorithm& rhs);
        bool       operator< (const Algorithm& rhs);
        bool       operator<=(const Algorithm& rhs);
        bool       operator> (const Algorithm& rhs);
        bool       operator>=(const Algorithm& rhs);

        static bool isValid(const char* algorithm);

        /**
         * @brief Get the Algorithm in a human readable (MST to LST) form. This
         * is reverse of the internal representation (LST to MST).
         * 
         * @return std::vector<Turn> 
         */
        std::vector<Turn> getAlgorithm() const;
        std::string getAlgorithmStr() const;

        /**
         * @brief Get the Algorithm Number. Algorithms are zero indexed.
         * 
         * @return unsigned long long int 
         */
        unsigned long long int getAlgorithmNumber() const;

        /**
         * @brief Set the Algorithm Number object starting from zero. This
         * call resets the algorithm and sets it explicity.
         * 
         * @param algNum 
         */
        void setAlgorithmNumber(unsigned long long int algNum);

        void setAlgorithmOrder(unsigned int algorithmOrder);
        unsigned int getAlgorithmOrder() const;

        /**
         * @brief Does the same thing as setAlogorithmNumber, but is faster if
         * the difference between the current algorithm number and the argument
         * algorithm number is small. Since algorithm subtraction is not (yet)
         * supported, this has the same affect as calling setAlgorithmNumber if
         * the difference is negative.
         * 
         * @param algNum The algorithm number to update to.
         */
        void incrementAlgorithmToAlgNum(unsigned long long int algNum);

        void reset();
        void addTurn(Turn turn);
        static char layerToChar(Layer layer);
        static Layer charToLayer(char lChar);

        /**
         * @brief Performs all redundancy checks.
         * 
         * @return true If this is a redundant algorith.
         * @return false If this is not a redundant algorithm.
         */
        bool isRedundant();

        /**
         * Returns true if the current algorithm contains at least one inverted
         * turn. Examples of an inverted turn are F F' or B' B, etc.
         */
        bool hasInversion();
        bool hasHiddenInversion();
        /**
         * Returns true if the current algorithm contains at least one instance
         * of three of the same move in a row.
         */
        bool hasTriple();
        bool hasHiddenTriple();

        /**
         * Expects a string of turns delimited by spaces. Valid values in the
         * string are spaces, layer letters, and single quotes. The parser is
         * smart enough to extract valid turns from malformed strings.
         *
         * A turn is a layer character (see Cube.h) and an optional single quote.
         * The optional single quote denotes an anti-clockwise turn. A turn that
         * does not include the single quote is assumed to be clockwise.
         */
        void setAlgorithm(const char* algorithm);
        void setAlgorithm(const std::vector<Turn> turns);
      
    private:
        static const unsigned int ALGORITHM_BASE = 12;

        /* @brief Internal represetation is LST at index 0. */
        std::vector<unsigned long long int> algorithm;
        unsigned int algorithmOrder = 0;
        unsigned long long int algorithmNumber = 0;

        void addToAlgorithm(const unsigned long long int addend);
        Turn getTurnForNumber(unsigned long long int number) const;
        unsigned int getNumberForTurn(Turn turn) const;
        unsigned int getOppositeFace(unsigned long long int face);
};

#endif // ALGORITHM_H
