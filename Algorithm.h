/**
 * License:
 *    MIT License
 *
 *    Copyright (c) 2019 Chuck Wolber
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a
 *    copy of this software and associated documentation files (the 
 *    "Software"), to deal in the Software without restriction, including
 *    without limitation the rights to use, copy, modify, merge, publish,
 *    distribute, sublicense, and/or sell copies of the Software, and to permit
 *    persons to whom the Software is furnished to do so, subject to the
 *    following conditions:
 * 
 *    The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 *    NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 *    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 *    USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * Description:
 *    This class is not thread safe.
 * 
 *    This class models an algorithm that can be applied to a Rubik's Cube
 *    (3x3x3). All algorithms are based on the QTM (Quarter Turn Metric) and
 *    are limited to clockwise and anticlockwise versions of the F, U, R, D, L,
 *    B layers. These limitations simplify modeling a Rubik's Cube in computer
 *    memory, without giving up performance.
 * 
 *    While humans find a great deal of benefit from using expanded shorthand
 *    notation (https://www.speedsolving.com/wiki/index.php/NxNxN_Notation),
 *    such notation makes computing algorithms much more computationally
 *    difficult on a Rubik's Cube.
 * 
 *    Patches are welcomed!
 * 
 * Bugs and Features:
 *    https://github.com/chuckwolber/Cube/issues
 *
 * Glossary of Terms:
 *    * Layer: See glossary in Cube.h for Layer definition.
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
#include "Cube.h"

class Algorithm {
   public:
      Algorithm();
      Algorithm(const std::vector<Turn> turns);
      Algorithm(const Algorithm& obj);

      Algorithm& operator= (const Algorithm& rhs);
      Algorithm& operator++();    // prefix increment
      Algorithm  operator++(int); // postfix increment
      Algorithm& operator+=(unsigned int x);
      bool       operator==(const Algorithm& rhs);
      bool       operator!=(const Algorithm& rhs);
      bool       operator< (const Algorithm& rhs);
      bool       operator<=(const Algorithm& rhs);
      bool       operator> (const Algorithm& rhs);
      bool       operator>=(const Algorithm& rhs);

      std::vector<Turn> getAlgorithm() const;

      void clear();
      void addTurn(Turn turn);

      /**
       * Returns true if the current algorithm contains at least one inverted
       * turn. Examples of an inverted turn are F F' or B' B, etc.
       */
      bool hasInversion();

      /**
       * Returns true if the current algorithm contains at least one instance
       * of three of the same move in a row.
       */
      bool hasTriple();

      /**
       * Expects a string of turns delimited by spaces. Valid values in the
       * string are spaces, layer letters, and single quotes. The parser is
       * smart enough to extract valid turns from malformed strings.
       * 
       * A turn is a layer character (see Cube.h) and an optional single quote.
       * The optional single quote denotes an anti-clockwise turn. A turn that
       * does not include the single quote is assumed to be clockwise
       * .
       */
      void setAlgorithm(const char *algorithm);
      void setAlgorithm(const std::vector<Turn> turns);
      
   private:
      static const unsigned int ALGORITHM_BASE = 12;
      std::vector<unsigned int> algorithm;

      void addToAlgorithm(const unsigned int addend);
      Turn getTurnForNumber(unsigned int number) const;
      unsigned int getNumberForTurn(Turn turn) const;
};

#endif // ALGORITHM_H
