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

#include "Algorithm.hpp"

Algorithm::Algorithm() {
    addTurn(initialTurn);
}

Algorithm::Algorithm(const unsigned long long int algNum) {
    setAlgorithmNumber(algNum);
}

Algorithm::Algorithm(const std::vector<Turn> turns) {
    setAlgorithm(turns);
}

Algorithm::Algorithm(const Algorithm& obj) {
    algorithm = obj.algorithm;
    algorithmNumber = obj.algorithmNumber;
    algorithmOrder = obj.algorithmOrder;
}

Algorithm::Algorithm(const char* algorithm) {
    setAlgorithm(algorithm);
}

Algorithm& Algorithm::operator=(const Algorithm& rhs) {
    if (&rhs != this) {
        algorithm = rhs.algorithm;
        algorithmNumber = rhs.algorithmNumber;
        algorithmOrder = rhs.algorithmOrder;
    }
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

Algorithm& Algorithm::operator+=(unsigned long long int x) {
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

unsigned long long int Algorithm::getAlgorithmNumber() const {
    return algorithmNumber;
}

void Algorithm::setAlgorithmNumber(unsigned long long int algNum) {
    algorithm.clear();
    algorithmNumber = 0;
    addTurn(initialTurn);
    addToAlgorithm(algNum);
}

void Algorithm::setAlgorithmOrder(unsigned int algorithmOrder) {
    this->algorithmOrder = algorithmOrder;
}

unsigned int Algorithm::getAlgorithmOrder() const {
    return algorithmOrder;
}

void Algorithm::incrementAlgorithmToAlgNum(unsigned long long int algNum) {
    if (algNum == algorithmNumber)
        return;

    if (algNum < algorithmNumber)
        setAlgorithmNumber(algNum);
    else
        addToAlgorithm(algNum - algorithmNumber);
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
    algorithmNumber = 0;
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
    algorithmNumber = 0;
    addTurn(initialTurn);
}

void Algorithm::addTurn(Turn turn) {
    std::vector<unsigned long long int>::iterator it = algorithm.begin();
    algorithm.insert(it, getNumberForTurn(turn));

    algorithmNumber = 0;
    unsigned long long int base = 1;
    for (unsigned long long int t : algorithm) {
        algorithmNumber += t*base;
        base *= ALGORITHM_BASE;
    }
}

void Algorithm::addToAlgorithm(unsigned long long int addend) {
    unsigned int index = 0;
    unsigned long long int fieldCarry = 0;

    unsigned long long int addendModulus;
    unsigned long long int fieldSum;
    unsigned long long int fieldValue;
   
    algorithmNumber += addend;
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
    for (size_t i=algorithm.size(); i > 0; i--) {
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
    for (unsigned long i=algorithm.size(); i > 0; i--)
        a.push_back(getTurnForNumber(algorithm[i-1]));
    return a;
}

bool Algorithm::isRedundant() {
    if (hasInversion())
        return true;
    if (hasHiddenInversion())
        return true;
    if (hasTriple())
        return true;
    if (hasHiddenTriple())
        return true;
    return false;
}

bool Algorithm::hasInversion() {
    if (algorithm.size() < 2)
        return false;

    /* X X' */
    for (unsigned int i = 0; i < (algorithm.size() - 1); i++) {
        if (algorithm.at(i) % 2 == 0 &&
            algorithm.at(i) == algorithm.at(i+1) - 1)
            return true;

        if (algorithm.at(i) % 2 == 1 &&
            algorithm.at(i) == algorithm.at(i+1) + 1)
            return true;
    }
    return false;
}

bool Algorithm::hasHiddenInversion() {
    if (algorithm.size() < 3)
        return false;

    /* X (Y | Y') X' */
    for (unsigned int i = 0; i < algorithm.size() - 2; i++) {
        unsigned int o1 = getOppositeFace(algorithm.at(i));
        unsigned int o2 = o1 + 1;

        if (algorithm.at(i+1) != o1 && algorithm.at(i+1) != o2)
            continue;

        if (algorithm.at(i) % 2 == 0 &&
            algorithm.at(i) == algorithm.at(i+2) - 1)
            return true;

        if (algorithm.at(i) % 2 == 1 &&
            algorithm.at(i) == algorithm.at(i+2) + 1)
            return true;
    }

    if (algorithm.size() < 4)
        return false;
    
    /* X (Y | Y') (Y | Y') X' */
    for (unsigned int i = 0; i < algorithm.size() - 3; i++) {
        unsigned int o1 = getOppositeFace(algorithm.at(i));
        unsigned int o2 = o1 + 1;

        if ((algorithm.at(i+1) != o1 && algorithm.at(i+1) != o2) ||
            (algorithm.at(i+2) != o1 && algorithm.at(i+2) != o2))
            continue;
        
        if (algorithm.at(i) % 2 == 0 &&
            algorithm.at(i) == algorithm.at(i+3) - 1)
            return true;

        if (algorithm.at(i) % 2 == 1 &&
            algorithm.at(i) == algorithm.at(i+3) + 1)
            return true;
    }

    return false;
}

bool Algorithm::hasTriple() {
    /* 0:0 - X X X */
    if (algorithm.size() < 3)
        return false;

    for (unsigned int i = 0; i <= (algorithm.size() - 3); i++) {
        if (algorithm.at(i)   == algorithm.at(i+1) &&
            algorithm.at(i+1) == algorithm.at(i+2))
            return true;
    }
    return false;
}

bool Algorithm::hasHiddenTriple() {
    if (algorithm.size() < 4)
        return false;

    for (unsigned int i = 0; i <= (algorithm.size() - 4); i++) {
        unsigned long long int c = algorithm.at(i);
        unsigned long long int o1 = getOppositeFace(algorithm.at(i));
        unsigned long long int o2 = o1 + 1;

        if (c != algorithm.at(i+3))
            continue;
        
        /* 0:1 - X X (Y | Y') X */
        if (c == algorithm.at(i+1)) {
            if (o1 != algorithm.at(i+2) && o2 != algorithm.at(i+2))
                continue;
            return true;
        }
        
        /* 1:0 - X (Y | Y') X X */
        if (c == algorithm.at(i+2)) {
            if (o1 != algorithm.at(i+1) && o2 != algorithm.at(i+1))
                continue;
            return true;
        }
    }

    if (algorithm.size() < 5)
        return false;
    
    for (unsigned int i = 0; i <= (algorithm.size() - 5); i++) {
        unsigned long long int o1 = getOppositeFace(algorithm.at(i));
        unsigned long long int o2 = o1 + 1;
        unsigned long long int c = algorithm.at(i);

        if (c != algorithm.at(i+4))
            continue;
        
        /* 0:2 - X X (Y | Y') (Y | Y') X */
        if (c == algorithm.at(i+1)) {
            if (o1 != algorithm.at(i+2) && o2 != algorithm.at(i+2))
                continue;
            if (o1 != algorithm.at(i+3) && o2 != algorithm.at(i+3))
                continue;
            return true;
        }

        /* 1:1 - X (Y | Y') X (Y | Y') X */
        if (c == algorithm.at(i+2)) {
            if (o1 != algorithm.at(i+1) && o2 != algorithm.at(i+1))
                continue;
            if (o1 != algorithm.at(i+3) && o2 != algorithm.at(i+3))
                continue;
            return true;
        }

        /* 2:0 - X (Y | Y') (Y | Y') X X */
        if (c == algorithm.at(i+3)) {
            if (o1 != algorithm.at(i+1) && o2 != algorithm.at(i+1))
                continue;
            if (o1 != algorithm.at(i+2) && o2 != algorithm.at(i+2))
                continue;
            return true;
        }
    }
    
    if (algorithm.size() < 6)
        return false;
    
    for (unsigned int i = 0; i <= (algorithm.size() - 6); i++) {
        unsigned long long int o1 = getOppositeFace(algorithm.at(i));
        unsigned long long int o2 = o1 + 1;
        unsigned long long int c = algorithm.at(i);

        if (c != algorithm.at(i+5))
            continue;

        /* 1:2 - X (Y | Y') X (Y | Y') (Y | Y') X */
        if (c == algorithm.at(i+2)) {
            if (o1 != algorithm.at(i+1) && o2 != algorithm.at(i+1))
                continue;
            if (o1 != algorithm.at(i+3) && o2 != algorithm.at(i+3))
                continue;
            if (o1 != algorithm.at(i+4) && o2 != algorithm.at(i+4))
                continue;
            return true;
        }

        /* 2:1 - X (Y | Y') (Y | Y') X (Y | Y') X */
        if (c == algorithm.at(i+3)) {
            if (o1 != algorithm.at(i+1) && o2 != algorithm.at(i+1))
                continue;
            if (o1 != algorithm.at(i+2) && o2 != algorithm.at(i+2))
                continue;
            if (o1 != algorithm.at(i+4) && o2 != algorithm.at(i+4))
                continue;
            return true;
        }
    }

    if (algorithm.size() < 7)
        return false;
    
    for (unsigned int i = 0; i <= (algorithm.size() - 7); i++) {
        unsigned long long int o1 = getOppositeFace(algorithm.at(i));
        unsigned long long int o2 = o1 + 1;
        unsigned long long int c = algorithm.at(i);

        if (c != algorithm.at(i+6))
            continue;
        
         /* 2:2 - X (Y | Y') (Y | Y') X (Y | Y') (Y | Y') X */
         if (c == algorithm.at(i+3)) {
            if (o1 != algorithm.at(i+1) && o2 != algorithm.at(i+1))
                continue;
            if (o1 != algorithm.at(i+2) && o2 != algorithm.at(i+2))
                continue;
            if (o1 != algorithm.at(i+4) && o2 != algorithm.at(i+4))
                continue;
            if (o1 != algorithm.at(i+5) && o2 != algorithm.at(i+5))
                continue;
            return true;
         }
    }

    return false;
}

unsigned int Algorithm::getOppositeFace(unsigned long long int face) {
    switch (face) {
        case 0:      // Front
        case 1:
         return 10;  // Back
         break;
      case 2:        // Up
      case 3:
         return 6;   // Down
         break;
      case 4:        // Right
      case 5:
         return 8;   // Left
         break;
      case 6:        // Down
      case 7:
         return 2;   // Up
         break;
      case 8:        // Left
      case 9:
         return 4;   // Right
         break;
      case 10:       // Back
      case 11:
         return 0;   // Front
         break;
      default:
         return 0;
         break;
    }
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

Turn Algorithm::getTurnForNumber(unsigned long long int number) const {
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
