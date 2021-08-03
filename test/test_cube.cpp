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

#include <cassert>
#include <iostream>
#include <vector>
#include "../Algorithm.h"
#include "../Cube.h"

void test_constructors();
void test_sized_constructors(unsigned int cubeSize);
void test_operators();
void test_getCubeSize();
void test_turns();

Cube getScrambled();
std::vector<CubieColor> getExpectedScrambled();
std::vector<CubieColor> getExpected(CubieColor referenceColor, 
                                    unsigned int cubeSize);
void setColors(std::vector<CubieColor>& cube,
               std::vector<CubieColor>colorList,
               unsigned int cubeSize);
void verify_cube(Cube cube, std::vector<CubieColor> expected);

int main() {
   test_constructors();
   test_operators();
   test_getCubeSize();
   test_turns();

   return 0;
}

void test_constructors() {
   std::cout << "Testing constructors... ";

   Cube c1;
   verify_cube(c1, getExpected(CubieColor::WHITE,  3));

   Cube c2(CubieColor::WHITE);
   Cube c3(CubieColor::BLUE);
   Cube c4(CubieColor::GREEN);
   Cube c5(CubieColor::ORANGE);
   Cube c6(CubieColor::RED);
   Cube c7(CubieColor::YELLOW);

   verify_cube(c2, getExpected(CubieColor::WHITE,  3));
   verify_cube(c3, getExpected(CubieColor::BLUE,   3));
   verify_cube(c4, getExpected(CubieColor::GREEN,  3));
   verify_cube(c5, getExpected(CubieColor::ORANGE, 3));
   verify_cube(c6, getExpected(CubieColor::RED,    3));
   verify_cube(c7, getExpected(CubieColor::YELLOW, 3));

   for (unsigned int i = 0; i < 10; i++)
      test_sized_constructors(i);

   std::cout << "Passed" << std::endl;
}

void test_sized_constructors(unsigned int cubeSize) {
   Cube c1(CubieColor::WHITE,  cubeSize);
   Cube c2(CubieColor::BLUE,   cubeSize);
   Cube c3(CubieColor::GREEN,  cubeSize);
   Cube c4(CubieColor::ORANGE, cubeSize);
   Cube c5(CubieColor::RED,    cubeSize);
   Cube c6(CubieColor::YELLOW, cubeSize);

   Cube c1_cp(c1);
   Cube c2_cp(c2);
   Cube c3_cp(c3);
   Cube c4_cp(c4);
   Cube c5_cp(c5);
   Cube c6_cp(c6);

   verify_cube(c1, getExpected(CubieColor::WHITE,  cubeSize));
   verify_cube(c2, getExpected(CubieColor::BLUE,   cubeSize));
   verify_cube(c3, getExpected(CubieColor::GREEN,  cubeSize));
   verify_cube(c4, getExpected(CubieColor::ORANGE, cubeSize));
   verify_cube(c5, getExpected(CubieColor::RED,    cubeSize));
   verify_cube(c6, getExpected(CubieColor::YELLOW, cubeSize));

   verify_cube(c1_cp, getExpected(CubieColor::WHITE,  cubeSize));
   verify_cube(c2_cp, getExpected(CubieColor::BLUE,   cubeSize));
   verify_cube(c3_cp, getExpected(CubieColor::GREEN,  cubeSize));
   verify_cube(c4_cp, getExpected(CubieColor::ORANGE, cubeSize));
   verify_cube(c5_cp, getExpected(CubieColor::RED,    cubeSize));
   verify_cube(c6_cp, getExpected(CubieColor::YELLOW, cubeSize));

   Cube c1_mv(std::move(c1));
   Cube c2_mv(std::move(c2));
   Cube c3_mv(std::move(c3));
   Cube c4_mv(std::move(c4));
   Cube c5_mv(std::move(c5));
   Cube c6_mv(std::move(c6));

   verify_cube(c1_mv, getExpected(CubieColor::WHITE,  cubeSize));
   verify_cube(c2_mv, getExpected(CubieColor::BLUE,   cubeSize));
   verify_cube(c3_mv, getExpected(CubieColor::GREEN,  cubeSize));
   verify_cube(c4_mv, getExpected(CubieColor::ORANGE, cubeSize));
   verify_cube(c5_mv, getExpected(CubieColor::RED,    cubeSize));
   verify_cube(c6_mv, getExpected(CubieColor::YELLOW, cubeSize));
}

void test_operators() {
   std::cout << "Testing operators... ";

   Cube c4(CubieColor::WHITE,  4);
   Cube c5(CubieColor::BLUE,   5);
   Cube c6(CubieColor::GREEN,  6);
   Cube c7(CubieColor::ORANGE, 7);
   Cube c8(CubieColor::RED,    8);
   Cube c9(CubieColor::YELLOW, 9);
   verify_cube(c4, getExpected(CubieColor::WHITE,  4));
   verify_cube(c5, getExpected(CubieColor::BLUE,   5));
   verify_cube(c6, getExpected(CubieColor::GREEN,  6));
   verify_cube(c7, getExpected(CubieColor::ORANGE, 7));
   verify_cube(c8, getExpected(CubieColor::RED,    8));
   verify_cube(c9, getExpected(CubieColor::YELLOW, 9));

   Cube c4_cp; c4_cp = c4; assert(c4_cp == c4);
   Cube c5_cp; c5_cp = c5; assert(c5_cp == c5);
   Cube c6_cp; c6_cp = c6; assert(c6_cp == c6);
   Cube c7_cp; c7_cp = c7; assert(c7_cp == c7);
   Cube c8_cp; c8_cp = c8; assert(c8_cp == c8);
   Cube c9_cp; c9_cp = c9; assert(c9_cp == c9);
   verify_cube(c4_cp, getExpected(CubieColor::WHITE,  4));
   verify_cube(c5_cp, getExpected(CubieColor::BLUE,   5));
   verify_cube(c6_cp, getExpected(CubieColor::GREEN,  6));
   verify_cube(c7_cp, getExpected(CubieColor::ORANGE, 7));
   verify_cube(c8_cp, getExpected(CubieColor::RED,    8));
   verify_cube(c9_cp, getExpected(CubieColor::YELLOW, 9));
   
   Cube c4_mv; c4_mv = std::move(c4);
   Cube c5_mv; c5_mv = std::move(c5);
   Cube c6_mv; c6_mv = std::move(c6);
   Cube c7_mv; c7_mv = std::move(c7);
   Cube c8_mv; c8_mv = std::move(c8);
   Cube c9_mv; c9_mv = std::move(c9);
   verify_cube(c4_mv, getExpected(CubieColor::WHITE,  4));
   verify_cube(c5_mv, getExpected(CubieColor::BLUE,   5));
   verify_cube(c6_mv, getExpected(CubieColor::GREEN,  6));
   verify_cube(c7_mv, getExpected(CubieColor::ORANGE, 7));
   verify_cube(c8_mv, getExpected(CubieColor::RED,    8));
   verify_cube(c9_mv, getExpected(CubieColor::YELLOW, 9));

   std::cout << "Passed" << std::endl;
}

void test_getCubeSize() {
   std::cout << "Testing getCubeSize... ";

   Cube c1;
   assert(c1.getCubeSize() == 3);

   for (unsigned int i = 2; i < 10; i++) {
      Cube c(CubieColor::WHITE, i);
      assert(c.getCubeSize() == i);
   }

   std::cout << "Passed" << std::endl;
}

void test_turns() {
   std::cout << "Testing turns... ";
   Cube c1(CubieColor::RED), c2(CubieColor::RED), c3(CubieColor::RED);

   assert(c1.isSolved());
   assert(c2.isSolved());
   assert(c3.isSolved());
   assert(c1 == c2);
   assert(c2 == c3);

   c1 = getScrambled();
   verify_cube(c1, getExpectedScrambled());
   assert(c1 != c2);
   assert(!c1.isSolved());
   
   Algorithm alg;
   alg.setAlgorithm("R' B U' L F' U F' D");
   c2.performAlgorithm(alg.getAlgorithm());
   assert(c2 == c1);
   assert(!c2.isSolved());
   verify_cube(c2, getExpectedScrambled());

   c3.turn({Layer::R, false});
   c3.turn({Layer::B, true});
   c3.turn({Layer::U, false});
   c3.turn({Layer::L, true});
   c3.turn({Layer::F, false});
   c3.turn({Layer::U, true});
   c3.turn({Layer::F, false});
   c3.turn({Layer::D, true});
   assert(c3 == c2);
   assert(!c3.isSolved());
   verify_cube(c3, getExpectedScrambled());

   alg.setAlgorithm("D' F U' F L' U B' R");
   c1.performAlgorithm(alg.getAlgorithm());
   c2.performAlgorithm(alg.getAlgorithm());
   c3.turn({Layer::D, false});
   c3.turn({Layer::F, true});
   c3.turn({Layer::U, false});
   c3.turn({Layer::F, true});
   c3.turn({Layer::L, false});
   c3.turn({Layer::U, true});
   c3.turn({Layer::B, false});
   c3.turn({Layer::R, true});
   verify_cube(c1, getExpected(CubieColor::RED, 3));
   verify_cube(c2, getExpected(CubieColor::RED, 3));
   verify_cube(c3, getExpected(CubieColor::RED, 3));

   assert(c1.isSolved());
   assert(c2.isSolved());
   assert(c3.isSolved());
   assert(c1 == c2);
   assert(c2 == c3);

   c1.turn({Layer::F, true}); assert(!c1.isSolved());
   c1.turn({Layer::F, false}); assert(c1.isSolved());
   c1.turn({Layer::U, true}); assert(!c1.isSolved());
   c1.turn({Layer::U, false}); assert(c1.isSolved());
   c1.turn({Layer::L, true}); assert(!c1.isSolved());
   c1.turn({Layer::L, false}); assert(c1.isSolved());
   c1.turn({Layer::R, true}); assert(!c1.isSolved());
   c1.turn({Layer::R, false}); assert(c1.isSolved());
   c1.turn({Layer::D, true}); assert(!c1.isSolved());
   c1.turn({Layer::D, false}); assert(c1.isSolved());
   c1.turn({Layer::B, true}); assert(!c1.isSolved());
   c1.turn({Layer::B, false}); assert(c1.isSolved());
   c1.turn({Layer::M, true}); assert(!c1.isSolved());
   c1.turn({Layer::M, false}); assert(c1.isSolved());
   c1.turn({Layer::E, true}); assert(!c1.isSolved());
   c1.turn({Layer::E, false}); assert(c1.isSolved());
   c1.turn({Layer::S, true}); assert(!c1.isSolved());
   c1.turn({Layer::S, false}); assert(c1.isSolved());

   c1.turn({Layer::M, true}); assert(!c1.isSolved());
   c1.turn({Layer::M, true}); assert(!c1.isSolved());
   c1.turn({Layer::M, true}); assert(!c1.isSolved());
   c1.turn({Layer::M, true}); assert(c1.isSolved());

   c1.turn({Layer::E, true}); assert(!c1.isSolved());
   c1.turn({Layer::E, true}); assert(!c1.isSolved());
   c1.turn({Layer::E, true}); assert(!c1.isSolved());
   c1.turn({Layer::E, true}); assert(c1.isSolved());

   c1.turn({Layer::S, true}); assert(!c1.isSolved());
   c1.turn({Layer::S, true}); assert(!c1.isSolved());
   c1.turn({Layer::S, true}); assert(!c1.isSolved());
   c1.turn({Layer::S, true}); assert(c1.isSolved());

   std::cout << "Passed" << std::endl;
}

/**
 * Returns a cube in the following arrangement:
 * 
 *       r o o
 *       b w o
 *       b y o
 * g w o w r b y b g w w w
 * g g w r r b y b y o o y
 * o o y g g b r g b y b y
 *       r r w
 *       g y w
 *       g r r
 */
Cube getScrambled() {
   Cube c(CubieColor::RED, 3);
   c.turn({Layer::R, false});
   c.turn({Layer::B, true});
   c.turn({Layer::U, false});
   c.turn({Layer::L, true});
   c.turn({Layer::F, false});
   c.turn({Layer::U, true});
   c.turn({Layer::F, false});
   c.turn({Layer::D, true});
   return c;
}

std::vector<CubieColor> getExpectedScrambled() {
   std::vector<CubieColor> tmp;
   for (int i = 0; i < 3*4*3*3; i++)
      tmp.push_back(CubieColor::NOCOLOR);

   /* Up Face */
   tmp.at(3) = CubieColor::RED;     tmp.at(4) = CubieColor::ORANGE;  tmp.at(5) = CubieColor::ORANGE;
   tmp.at(15) = CubieColor::BLUE;   tmp.at(16) = CubieColor::WHITE;  tmp.at(17) = CubieColor::ORANGE;
   tmp.at(27) = CubieColor::BLUE;   tmp.at(28) = CubieColor::YELLOW; tmp.at(29) = CubieColor::ORANGE;

   /* Left Face */
   tmp.at(36) = CubieColor::GREEN;  tmp.at(37) = CubieColor::WHITE;  tmp.at(38) = CubieColor::ORANGE;
   tmp.at(48) = CubieColor::GREEN;  tmp.at(49) = CubieColor::GREEN;  tmp.at(50) = CubieColor::WHITE;
   tmp.at(60) = CubieColor::ORANGE; tmp.at(61) = CubieColor::ORANGE; tmp.at(62) = CubieColor::YELLOW;

   /* Front Face */
   tmp.at(39) = CubieColor::WHITE;  tmp.at(40) = CubieColor::RED;    tmp.at(41) = CubieColor::BLUE;
   tmp.at(51) = CubieColor::RED;    tmp.at(52) = CubieColor::RED;    tmp.at(53) = CubieColor::BLUE;
   tmp.at(63) = CubieColor::GREEN;  tmp.at(64) = CubieColor::GREEN;  tmp.at(65) = CubieColor::BLUE;

   /* Right Face */
   tmp.at(42) = CubieColor::YELLOW; tmp.at(43) = CubieColor::BLUE;   tmp.at(44) = CubieColor::GREEN;
   tmp.at(54) = CubieColor::YELLOW; tmp.at(55) = CubieColor::BLUE;   tmp.at(56) = CubieColor::YELLOW;
   tmp.at(66) = CubieColor::RED;    tmp.at(67) = CubieColor::GREEN;  tmp.at(68) = CubieColor::BLUE;

   /* Back Face */
   tmp.at(45) = CubieColor::WHITE;  tmp.at(46) = CubieColor::WHITE;  tmp.at(47) = CubieColor::WHITE;
   tmp.at(57) = CubieColor::ORANGE; tmp.at(58) = CubieColor::ORANGE; tmp.at(59) = CubieColor::YELLOW;
   tmp.at(69) = CubieColor::YELLOW; tmp.at(70) = CubieColor::BLUE;   tmp.at(71) = CubieColor::YELLOW;

   /* Down Face */
   tmp.at(75) = CubieColor::RED;    tmp.at(76) = CubieColor::RED;    tmp.at(77) = CubieColor::WHITE;
   tmp.at(87) = CubieColor::GREEN;  tmp.at(88) = CubieColor::YELLOW; tmp.at(89) = CubieColor::WHITE;
   tmp.at(99) = CubieColor::GREEN;  tmp.at(100) = CubieColor::RED;   tmp.at(101) = CubieColor::RED;

   return tmp;
}

std::vector<CubieColor> getExpected(CubieColor referenceColor,
                                    unsigned int cubeSize) {
   if (cubeSize < 2)
      cubeSize = 2;

   std::vector<CubieColor> colorList;
   std::vector<CubieColor> tmp;
   for (unsigned int i = 0; i < cubeSize*4*cubeSize*3; i++)
      tmp.push_back(CubieColor::NOCOLOR);

   switch (referenceColor) {
      case CubieColor::BLUE:
         colorList.push_back(CubieColor::WHITE);
         colorList.push_back(CubieColor::RED);
         colorList.push_back(CubieColor::BLUE);
         colorList.push_back(CubieColor::ORANGE);
         colorList.push_back(CubieColor::GREEN);
         colorList.push_back(CubieColor::YELLOW);
         break;
      case CubieColor::GREEN:
         colorList.push_back(CubieColor::WHITE);
         colorList.push_back(CubieColor::ORANGE);
         colorList.push_back(CubieColor::GREEN);
         colorList.push_back(CubieColor::RED);
         colorList.push_back(CubieColor::BLUE);
         colorList.push_back(CubieColor::YELLOW);
         break;
      case CubieColor::ORANGE:
         colorList.push_back(CubieColor::WHITE);
         colorList.push_back(CubieColor::BLUE);
         colorList.push_back(CubieColor::ORANGE);
         colorList.push_back(CubieColor::GREEN);
         colorList.push_back(CubieColor::RED);
         colorList.push_back(CubieColor::YELLOW);
         break;
      case CubieColor::RED:
         colorList.push_back(CubieColor::WHITE);
         colorList.push_back(CubieColor::GREEN);
         colorList.push_back(CubieColor::RED);
         colorList.push_back(CubieColor::BLUE);
         colorList.push_back(CubieColor::ORANGE);
         colorList.push_back(CubieColor::YELLOW);
         break;
      case CubieColor::WHITE:
         colorList.push_back(CubieColor::GREEN);
         colorList.push_back(CubieColor::RED);
         colorList.push_back(CubieColor::WHITE);
         colorList.push_back(CubieColor::ORANGE);
         colorList.push_back(CubieColor::YELLOW);
         colorList.push_back(CubieColor::BLUE);
         break;
      case CubieColor::YELLOW:
         colorList.push_back(CubieColor::GREEN);
         colorList.push_back(CubieColor::ORANGE);
         colorList.push_back(CubieColor::YELLOW);
         colorList.push_back(CubieColor::RED);
         colorList.push_back(CubieColor::WHITE);
         colorList.push_back(CubieColor::BLUE);
         break;
      default:
         break;
   }
   setColors(tmp, colorList, cubeSize);

   return tmp;
}

void setColors(std::vector<CubieColor>& cube,
               std::vector<CubieColor>colorList,
               unsigned int cubeSize) {
   unsigned int i = 0;

   /* Up Face */
   for (unsigned int j = 0; j < cubeSize; j++) {
      for (unsigned int k = 0; k < cubeSize; k++)
         i++;
      for (unsigned int k = 0; k < cubeSize; k++)
         cube.at(i++) = colorList.at(0);
      for (unsigned int k = 0; k < cubeSize; k++)
         i++;
      for (unsigned int k = 0; k < cubeSize; k++)
         i++;
   }

   /* Left, Front, Right, Back Faces */
   for (unsigned int j = 0; j < cubeSize; j++) {
      for (unsigned int k = 0; k < cubeSize; k++)
         cube.at(i++) = colorList.at(1);
      for (unsigned int k = 0; k < cubeSize; k++)
         cube.at(i++) = colorList.at(2);
      for (unsigned int k = 0; k < cubeSize; k++)
         cube.at(i++) = colorList.at(3);
      for (unsigned int k = 0; k < cubeSize; k++)
         cube.at(i++) = colorList.at(4);
   }

   /* Down Face */
   for (unsigned int j = 0; j < cubeSize; j++) {
      for (unsigned int k = 0; k < cubeSize; k++)
         i++;
      for (unsigned int k = 0; k < cubeSize; k++)
         cube.at(i++) = colorList.at(5);
      for (unsigned int k = 0; k < cubeSize; k++)
         i++;
      for (unsigned int k = 0; k < cubeSize; k++)
         i++;
   }
}

void verify_cube(Cube cube, std::vector<CubieColor> expected) {
   std::vector<CubieColor> result = cube.getCube();
   assert(result.size() == expected.size());
   for (unsigned int i = 0; i < result.size(); i++)
      assert(result.at(i) == expected.at(i));
}
