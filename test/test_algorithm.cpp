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

#include <cassert>
#include <iostream>
#include "../Algorithm.h"
#include "../Cube.h"

void test_constructors();
void test_inequalities();
void test_inequalities(unsigned int val);
void test_sequences();
void test_skip();
void test_addTurn();
void test_setAlgorithm();
void test_inversions();
void test_triples();

void verify_turns(std::vector<Turn> results, std::vector<Turn> expected);

int main() {
   test_constructors();
   test_inequalities();
   test_sequences();
   test_skip();
   test_addTurn();
   test_setAlgorithm();
   test_inversions();
   test_triples();

   return 0;
}

void test_constructors() {
   std::cout << "Testing constructors... ";

   std::vector<Turn> init_alg = {{Layer::D,  true}, 
                                 {Layer::F, false}, 
                                 {Layer::R, false}};

   Algorithm alg1;
   assert(alg1.getAlgorithm().size() == 0);

   Algorithm alg2(init_alg);
   assert(alg2.getAlgorithm().size() == init_alg.size());
   verify_turns(init_alg, alg2.getAlgorithm());

   Algorithm alg3(alg2);
   assert(alg3.getAlgorithm().size() == init_alg.size());
   verify_turns(init_alg, alg3.getAlgorithm());

   alg2.clear();
   assert(alg3 != alg2);

   Algorithm alg4 = alg3;
   assert(alg4.getAlgorithm().size() == init_alg.size());
   verify_turns(init_alg, alg4.getAlgorithm());

   alg3.clear();
   assert(alg4 != alg3);

   alg3 = alg2;
   assert(alg3 == alg2);

   std::cout << "Passed" << std::endl;
}

void test_inequalities() {
   std::cout << "Testing inequalities... ";

   unsigned int val = 1;
   test_inequalities(val + 2);
   test_inequalities(val *= 12 + 3);
   test_inequalities(val *= 12 + 4);
   test_inequalities(val *= 12 + 5);
   test_inequalities(val *= 12 + 6); 

   std::cout << "Passed" << std::endl;
}

void test_inequalities(unsigned int val) {
   Algorithm alg_1;
   Algorithm alg_2;

   alg_1 += val;
   alg_2 += val; 
   assert(alg_1 == alg_2);
   assert(alg_1 <= alg_2);
   assert(alg_2 >= alg_2);
   verify_turns(alg_1.getAlgorithm(), alg_2.getAlgorithm());

   alg_2 +=1;
   assert(alg_1 != alg_2);
   assert(alg_1  < alg_2);
   assert(alg_1 <= alg_2);
   assert(alg_2  > alg_1);
   assert(alg_2 >= alg_1);
}

void test_sequences() {
   std::cout << "Testing sequences... ";

   Algorithm alg;
   std::vector<Turn> results;
   std::vector<Turn> expected = {
      {Layer::F, true}, {Layer::F, false}, {Layer::U, true}, {Layer::U, false},
      {Layer::R, true}, {Layer::R, false}, {Layer::D, true}, {Layer::D, false},
      {Layer::L, true}, {Layer::L, false}, {Layer::B, true}, {Layer::B, false},

      {Layer::F, true}, {Layer::F,  true}, {Layer::F, true}, {Layer::F, false},
      {Layer::F, true}, {Layer::U,  true}, {Layer::F, true}, {Layer::U, false},
      {Layer::F, true}, {Layer::R,  true}, {Layer::F, true}, {Layer::R, false},
      {Layer::F, true}, {Layer::D,  true}, {Layer::F, true}, {Layer::D, false},
      {Layer::F, true}, {Layer::L,  true}, {Layer::F, true}, {Layer::L, false},
      {Layer::F, true}, {Layer::B,  true}, {Layer::F, true}, {Layer::B, false},
   };

   for (int i = 0; i < 24; i++) {
      alg++;
      for (Turn t : alg.getAlgorithm())
         results.push_back(t);
   }
   verify_turns(results, expected);
   results.clear();
   alg.clear();

   for (int i = 0; i < 24; i++) {
      ++alg;
      for (Turn t : alg.getAlgorithm())
         results.push_back(t);
   }
   verify_turns(results, expected);
   results.clear();
   alg.clear();

   for (int i = 0; i < 24; i++) {
      alg += 1;
      for (Turn t : alg.getAlgorithm())
         results.push_back(t);
   }
   verify_turns(results, expected);
   results.clear();
   alg.clear();
   
   std::cout << "Passed" << std::endl;
}

void test_skip() {
   std::cout << "Testing skip... ";

   Algorithm alg;
   std::vector<Turn> results;
   std::vector<Turn> expected = {
      {Layer::D, true},  {Layer::F, true},  {Layer::F, false}, {Layer::F, true},
      {Layer::L, true},  {Layer::F, false}, {Layer::U, false}, {Layer::F, false},
      {Layer::B, true},  {Layer::U, true},  {Layer::R, false}, {Layer::U, false},
      {Layer::F, true},  {Layer::U, false}, {Layer::D, false}, {Layer::R, true},
      {Layer::U, true},  {Layer::R, true},  {Layer::L, false}, {Layer::R, false},
      {Layer::R, true},  {Layer::R, false}, {Layer::B, false}, {Layer::D, true},
      {Layer::D, true},  {Layer::D, false}, {Layer::F, false}, {Layer::D, false},
      {Layer::L, true},  {Layer::L, true},  {Layer::U, false}, {Layer::L, true},
      {Layer::B, true},  {Layer::L, false}, {Layer::R, false}, {Layer::B, true},
      {Layer::F, true},  {Layer::B, true},  {Layer::D, false}, {Layer::B, false},
      {Layer::U, true},  {Layer::B, false}, {Layer::L, false}, {Layer::F, true},
      {Layer::F, true},  {Layer::R, true},  {Layer::F, true},  {Layer::F, true},
      {Layer::B, false}, {Layer::F, true},  {Layer::F, false}, {Layer::D, true},
   };

   for (int i = 0; i < 25; i++) {
      alg += 7;
      for (Turn t : alg.getAlgorithm())
         results.push_back(t);
   }
   verify_turns(results, expected);

   std::cout << "Passed" << std::endl;
}

void test_addTurn() {
   std::cout << "Testing add turn... ";

   Turn t1 = {Layer::D, true};
   Turn t2 = {Layer::F, true};
   Turn t3 = {Layer::L, true};
   Algorithm alg;

   verify_turns(alg.getAlgorithm(), {});
   alg.addTurn(t1);
   verify_turns(alg.getAlgorithm(), {t1});
   alg.addTurn(t2);
   verify_turns(alg.getAlgorithm(), {t1, t2});
   alg.addTurn(t3);
   verify_turns(alg.getAlgorithm(), {t1, t2, t3});

   std::cout << "Passed" << std::endl;
}

void test_setAlgorithm() {
   std::cout << "Testing set algorithm... ";

   std::vector<Turn> algorithm = {
      {Layer::L, true},  {Layer::F, false}, 
      {Layer::U, false}, {Layer::F, false}
   };

   Algorithm alg_1(algorithm);
   Algorithm alg_2;

   alg_2.setAlgorithm("L F' U' F'");
   verify_turns(alg_1.getAlgorithm(), algorithm);
   verify_turns(alg_2.getAlgorithm(), alg_1.getAlgorithm());

   std::cout << "Passed" << std::endl;
}

void test_inversions() {
   std::cout << "Testing inversions... ";

   Algorithm alg_1({{Layer::L, true}, {Layer::F, true}});
   
   assert(!alg_1.hasInversion());
   alg_1 += 9;
   assert(alg_1.hasInversion());
   verify_turns(alg_1.getAlgorithm(), 
                 {{Layer::L, true}, {Layer::L, false}});

   Algorithm alg_2({{Layer::L, true}, {Layer::L, true}, {Layer::F, true}});
   assert(!alg_2.hasInversion());
   alg_2 += 12;
   assert(alg_2.hasInversion());
   verify_turns(alg_2.getAlgorithm(), 
                 {{Layer::L, true}, {Layer::L, false}, {Layer::F, true}});

   std::cout << "Passed" << std::endl;
}

void test_triples() {
   std::cout << "Testing triples... ";

   Algorithm alg_1;
   alg_1.addTurn({Layer::R, true});
   alg_1.addTurn({Layer::R, true});
   alg_1.addTurn({Layer::R, true});
   alg_1.addTurn({Layer::R, true});
   assert(alg_1.hasTriple());

   Algorithm alg_2;
   alg_2.addTurn({Layer::R, false});
   alg_2.addTurn({Layer::R, false});
   alg_2.addTurn({Layer::R, true});
   alg_2.addTurn({Layer::R, true});
   assert(!alg_2.hasTriple());
   alg_2.addTurn({Layer::R, true});
   alg_2.addTurn({Layer::F, true});
   alg_2.addTurn({Layer::U, true});
   alg_2.addTurn({Layer::B, true});
   assert(alg_2.hasTriple());

   Algorithm alg_3;
   alg_3.addTurn({Layer::R, false});
   alg_3.addTurn({Layer::U, true});
   alg_3.addTurn({Layer::R, false});
   alg_3.addTurn({Layer::B, true});
   alg_3.addTurn({Layer::R, false});
   alg_3.addTurn({Layer::F, true});
   alg_3.addTurn({Layer::U, false});
   alg_3.addTurn({Layer::B, false});
   assert(!alg_3.hasTriple());

   alg_3.addTurn({Layer::U, true});
   alg_3.addTurn({Layer::U, true});
   alg_3.addTurn({Layer::U, true});
   alg_3.addTurn({Layer::U, true});

   alg_3.addTurn({Layer::R, false});
   alg_3.addTurn({Layer::F, true});
   alg_3.addTurn({Layer::U, false});
   alg_3.addTurn({Layer::B, true});
   assert(alg_3.hasTriple());

   std::cout << "Passed" << std::endl;
}

void verify_turns(std::vector<Turn> results, std::vector<Turn> expected) {
   assert(results.size() == expected.size());
   for (int i = 0; i < expected.size(); i++) {
      assert(expected.at(i).layer     == results.at(i).layer);
      assert(expected.at(i).clockwise == results.at(i).clockwise);
   }
}
