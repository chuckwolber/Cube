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

#include "Algorithm.h"

Algorithm::Algorithm() {
    addTurn(initialTurn);
}

Algorithm::Algorithm(const std::vector<Turn> turns) {
    setAlgorithm(turns);
}

Algorithm::Algorithm(const Algorithm& obj) {
    algorithm = obj.algorithm;
}

Algorithm::Algorithm(const char* algorithm) {
    setAlgorithm(algorithm);
}

Algorithm& Algorithm::operator=(const Algorithm& rhs) {
    if (&rhs != this)
        algorithm = rhs.algorithm;
    return *this;
}

Algorithm& Algorithm::operator++() {
    addToAlgorithm(1);
    return *this;
}

Algorithm Algorithm::operator++(int) {
    Algorithm temp(*this);
    ++(*this);
    return temp;
}

Algorithm& Algorithm::operator+=(unsigned int x) {
    addToAlgorithm(x);
    return *this;
}

bool Algorithm::operator==(const Algorithm& rhs) {
    return algorithm == rhs.algorithm;
}

bool Algorithm::operator!=(const Algorithm& rhs) {
    return algorithm != rhs.algorithm;
}

bool Algorithm::operator<(const Algorithm& rhs) {
    if (algorithm.size() > rhs.algorithm.size())
        return false;
    if (algorithm.size() < rhs.algorithm.size())
        return true;
    for (ssize_t i = (ssize_t)algorithm.size() - 1; i >= 0; i--) {
        if (algorithm.at((size_t)i) == rhs.algorithm.at((size_t)i))
            continue;
        if (algorithm.at((size_t)i) < rhs.algorithm.at((size_t)i))
            return true;
        return false;
    }
    return false;
}

bool Algorithm::operator<=(const Algorithm& rhs) {
    if (*this == rhs || *this < rhs)
        return true;
    return false;
}

bool Algorithm::operator>(const Algorithm& rhs) {
    return !(*this <= rhs);
}

bool Algorithm::operator>=(const Algorithm& rhs) {
    if (*this > rhs || *this == rhs)
        return true;
    return false;
}

bool Algorithm::isValid(const char* algorithm) {
    if (algorithm == nullptr)
        return false;
    
    bool inTurn = false;
    Layer layer;

    while (*algorithm != '\0') {
        layer = Algorithm::charToLayer(*algorithm);
        if (inTurn) {
            if (*algorithm != ' ' && *algorithm != '\'')
                return false;
            if (*algorithm == ' ')
                inTurn = false;
        } else if (layer != Layer::NOLAYER) {
            inTurn = true;
        } else {
            return false;
        }
        algorithm++;
    }
    return true;
}

void Algorithm::setAlgorithm(const std::vector<Turn> turns) {
    algorithm.clear();
    for (Turn turn : turns)
        addTurn(turn);
}

void Algorithm::setAlgorithm(const char *algorithm) {
    if (!isValid(algorithm))
        return;
    
    this->algorithm.clear();
    Turn *turn = nullptr;
    Layer layer;

    while (*algorithm != '\0') {
        layer = Algorithm::charToLayer(*algorithm);
        if (turn != nullptr) {
            if (*algorithm == ' ') {
                addTurn(*turn);
                turn = nullptr;
            } else if (*algorithm == '\'') {
                turn->clockwise = false;
                addTurn(*turn);
                turn = nullptr;
            }
        } else if (layer != Layer::NOLAYER) {
            turn = new Turn;
            turn->clockwise = true;
            turn->layer = layer;
        }
        algorithm++;
    }
   
    if (turn != nullptr)
        addTurn(*turn);
}

void Algorithm::reset() {
    algorithm.clear();
    addTurn(initialTurn);
}

void Algorithm::addTurn(Turn turn) {
    std::vector<unsigned int>::iterator it = algorithm.begin();
    algorithm.insert(it, getNumberForTurn(turn));
}

void Algorithm::addToAlgorithm(unsigned int addend) {
    unsigned int index = 0;
    unsigned int fieldCarry = 0;

    unsigned int addendModulus;
    unsigned int fieldSum;
    unsigned int fieldValue;
   
    while (addend > 0 || fieldCarry > 0) {
        if (algorithm.size() < index + 1) {
            algorithm.push_back(0);
            if (addend != 0)
                addend--;
            else if (fieldCarry != 0)
                fieldCarry--;
        }

        addendModulus = addend % ALGORITHM_BASE;
        addend = (addend - addendModulus) / ALGORITHM_BASE;

        fieldSum = algorithm.at(index) + addendModulus + fieldCarry;
        fieldValue = fieldSum % ALGORITHM_BASE;
        fieldCarry = (fieldSum - fieldValue) / ALGORITHM_BASE;

        algorithm.at(index++) = fieldValue;
    }
}

std::string Algorithm::getAlgorithmStr() const {
    std::string result = "";
    for (unsigned long i=algorithm.size(); i>0; i--) {
        Turn t = getTurnForNumber(algorithm[i-1]);
        result += layerToChar(t.layer);

        if (!t.clockwise)
            result += "\'";
        if (i > 1)
            result += " ";
    }

    return result;
}

std::vector<Turn> Algorithm::getAlgorithm() const {
    std::vector<Turn> a;
    for (unsigned long i=algorithm.size(); i>0; i--)
        a.push_back(getTurnForNumber(algorithm[i-1]));
    return a;
}

bool Algorithm::hasInversion() {
    if (algorithm.size() == 1)
        return false;

    for (unsigned int i=0; i<(algorithm.size() - 1); i++) {
        if (algorithm.at(i) % 2 == 0 &&
            algorithm.at(i) == algorithm.at(i+1) - 1)
            return true;

        if (algorithm.at(i) % 2 == 1 &&
            algorithm.at(i) == algorithm.at(i+1) + 1)
            return true;
    }
    return false;
}

bool Algorithm::hasTriple() {
    if (algorithm.size() < 3)
        return false;

    for (unsigned int i=0; i<=(algorithm.size() - 3); i++) {
        if (algorithm.at(i)   == algorithm.at(i+1) &&
            algorithm.at(i+1) == algorithm.at(i+2))
            return true;
    }

    return false;
}

unsigned int Algorithm::getNumberForTurn(Turn turn) const {
   unsigned int number;
   switch(turn.layer) {
      case Layer::F:
         number = 0;
         break;
      case Layer::U:
         number = 2;
         break;
      case Layer::R:
         number = 4;
         break;
      case Layer::D:
         number = 6;
         break;
      case Layer::L:
         number = 8;
         break;
      case Layer::B:
         number = 10;
         break;
      default:
         number = 0;
         break;
   }

   if (!turn.clockwise)
      number++;
   return number;
}

Turn Algorithm::getTurnForNumber(unsigned int number) const {
   number = number % ALGORITHM_BASE;
   switch (number) {
      case 0:
         return {Layer::F, true};
         break;
      case 1:
         return {Layer::F, false};
         break;
      case 2:
         return {Layer::U, true};
         break;
      case 3:
         return {Layer::U, false};
         break;
      case 4:
         return {Layer::R, true};
         break;
      case 5:
         return {Layer::R, false};
         break;
      case 6:
         return {Layer::D, true};
         break;
      case 7:
         return {Layer::D, false};
         break;
      case 8:
         return {Layer::L, true};
         break;
      case 9:
         return {Layer::L, false};
         break;
      case 10:
         return {Layer::B, true};
         break;
      case 11:
         return {Layer::B, false};
         break;
      default:
         return {Layer::F, true};
         break;
   }
}

char Algorithm::layerToChar(Layer layer) {
   switch (layer) {
      case Layer::F:
         return 'F';
         break;
      case Layer::U:
         return 'U';
         break;
      case Layer::R:
         return 'R';
         break;
      case Layer::D:
         return 'D';
         break;
      case Layer::L:
         return 'L';
         break;
      case Layer::B:
         return 'B';
         break;
      case Layer::M:
         return 'M';
         break;
      case Layer::E:
         return 'E';
         break;
      case Layer::S:
         return 'S';
         break;
      default: // Keep the GNU happy.
         return 'X';
   }
}

Layer Algorithm::charToLayer(char lChar) {
   switch (lChar) {
      case 'F':
         return Layer::F;
         break;
      case 'U':
         return Layer::U;
         break;
      case 'R':
         return Layer::R;
         break;
      case 'D':
         return Layer::D;
         break;
      case 'L':
         return Layer::L;
         break;
      case 'B':
         return Layer::B;
         break;
      case 'M':
         return Layer::M;
         break;
      case 'E':
         return Layer::E;
         break;
      case 'S':
         return Layer::S;
         break;
      default:
         return Layer::NOLAYER;
   }
}
