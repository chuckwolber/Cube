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
#include "../Algorithm.hpp"
#include "../Cube.hpp"

void test_constructors();
void test_inequalities();
void test_inequalities(unsigned int val);
void test_sequences();
void test_skip();
void test_addTurn();
void test_setAlgorithm();
void test_inversions();
void test_hidden_inversions();
void test_triples();
void test_hidden_triples();
void test_string();

void verify_turns(std::vector<Turn> results, std::vector<Turn> expected);

int main() {
    test_constructors();
    test_inequalities();
    test_sequences();
    test_skip();
    test_addTurn();
    test_setAlgorithm();
    test_inversions();
    test_hidden_inversions();
    test_triples();
    test_hidden_triples();
    test_string();

    return 0;
}

void test_constructors() {
    std::cout << "Testing constructors... ";

    std::vector<Turn> init_alg = {{Layer::D,  true}, 
                                  {Layer::F, false}, 
                                  {Layer::R, false}};

    Algorithm alg1;
    assert(alg1.getAlgorithm().size() == 1);

    Algorithm alg2(init_alg);
    assert(alg2.getAlgorithm().size() == init_alg.size());
    verify_turns(init_alg, alg2.getAlgorithm());

    Algorithm alg3(alg2);
    assert(alg3.getAlgorithm().size() == init_alg.size());
    verify_turns(init_alg, alg3.getAlgorithm());

    alg2.reset();
    assert(alg3 != alg2);

    Algorithm alg4 = alg3;
    assert(alg4.getAlgorithm().size() == init_alg.size());
    verify_turns(init_alg, alg4.getAlgorithm());

    alg3.reset();
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
       {Layer::F, true}, {Layer::B,  true}, {Layer::F, true}, {Layer::B, false}
    };

    std::cout << " t1 ";
    for (int i = 0; i < 24; i++) {
        for (Turn t : alg.getAlgorithm())
            results.push_back(t);
        alg++;
    }
    verify_turns(results, expected);
    results.clear();
    alg.reset();

    std::cout << " t2 ";
    for (int i = 0; i < 24; i++) {
        for (Turn t : alg.getAlgorithm())
            results.push_back(t);
        ++alg;
    }
    verify_turns(results, expected);
    results.clear();
    alg.reset();

    std::cout << " t3 ";
    for (int i = 0; i < 24; i++) {
        for (Turn t : alg.getAlgorithm())
            results.push_back(t);
        alg += 1;
    }
    verify_turns(results, expected);
    results.clear();
    alg.reset();
   
    std::cout << "Passed" << std::endl;
}

void test_skip() {
    std::cout << "Testing skip... ";

    Algorithm alg;
    std::vector<Turn> results;
    std::vector<Turn> expected = {
        {Layer::F, true},  {Layer::D, false}, {Layer::F, true},  {Layer::U, true},
        {Layer::F, true},  {Layer::L, false}, {Layer::F, false}, {Layer::R, true},
        {Layer::F, false}, {Layer::B, false}, {Layer::U, true},  {Layer::D, true},
        {Layer::U, false}, {Layer::F, false}, {Layer::U, false}, {Layer::L, true},
        {Layer::R, true},  {Layer::U, false}, {Layer::R, true},  {Layer::B, true},
        {Layer::R, false}, {Layer::R, false}, {Layer::D, true},  {Layer::F, true},
        {Layer::D, true},  {Layer::D, false}, {Layer::D, false}, {Layer::U, true},
        {Layer::D, false}, {Layer::L, false}, {Layer::L, true},  {Layer::R, true},
        {Layer::L, true},  {Layer::B, false}, {Layer::L, false}, {Layer::D, true},
        {Layer::B, true},  {Layer::F, false}, {Layer::B, true},  {Layer::L, true},
        {Layer::B, false}, {Layer::U, false}, {Layer::B, false}, {Layer::B, true},
        {Layer::F, true},  {Layer::F, true},  {Layer::R, false}, {Layer::F, true},
        {Layer::F, false}, {Layer::F, true}
    };

    for (int i = 0; i < 25; i++) {
        for (Turn t : alg.getAlgorithm())
            results.push_back(t);
        alg += 7;
    }
    verify_turns(results, expected);

    std::cout << "Passed" << std::endl;
}

void test_addTurn() {
    std::cout << "Testing add turn... ";

    Turn init = {Layer::F, true};
    Turn t1 = {Layer::D, true};
    Turn t2 = {Layer::F, true};
    Turn t3 = {Layer::L, true};
    Algorithm alg;

    std::cout << " t0 ";
    verify_turns(alg.getAlgorithm(), {init});

    std::cout << " t1 ";
    alg.addTurn(t1);
    verify_turns(alg.getAlgorithm(), {init, t1});

    std::cout << " t2 ";
    alg.addTurn(t2);
    verify_turns(alg.getAlgorithm(), {init, t1, t2});

    std::cout << " t3 ";
    alg.addTurn(t3);
    verify_turns(alg.getAlgorithm(), {init, t1, t2, t3});

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
    verify_turns(alg_1.getAlgorithm(), {{Layer::L, true}, {Layer::L, false}});

    Algorithm alg_2({{Layer::L, true}, {Layer::L, true}, {Layer::F, true}});
    assert(!alg_2.hasInversion());
    alg_2 += 12;
    assert(alg_2.hasInversion());
    verify_turns(alg_2.getAlgorithm(), 
                    {{Layer::L, true}, {Layer::L, false}, {Layer::F, true}});

    Algorithm alg_3({{Layer::F, true}, {Layer::F, false}}); 
    assert(alg_3.hasInversion());

    Algorithm alg_4({{Layer::F, false}, {Layer::F, true}}); 
    assert(alg_4.hasInversion());

    Algorithm alg_5({{Layer::B, true}, {Layer::B, false}}); 
    assert(alg_5.hasInversion());

    Algorithm alg_6({{Layer::B, false}, {Layer::B, true}}); 
    assert(alg_6.hasInversion());

    std::cout << "Passed" << std::endl;
}

void test_hidden_inversions() {
    std::cout << "Testing hidden inversions... ";

    Algorithm alg_1;
    alg_1.addTurn({Layer::R, true});
    alg_1.addTurn({Layer::F, true});
    alg_1.addTurn({Layer::R, false});
    assert(!alg_1.hasHiddenInversion());

    Algorithm alg_2;
    alg_2.addTurn({Layer::R, true});
    alg_2.addTurn({Layer::L, true});
    alg_2.addTurn({Layer::R, false});
    assert(alg_2.hasHiddenInversion());

    Algorithm alg_3;
    alg_3.addTurn({Layer::R, true});
    alg_3.addTurn({Layer::L, true});
    alg_3.addTurn({Layer::L, true});
    alg_3.addTurn({Layer::R, false});
    assert(alg_3.hasHiddenInversion());

    Algorithm alg_4;
    alg_4.addTurn({Layer::R, true});
    alg_4.addTurn({Layer::F, true});
    alg_4.addTurn({Layer::R, false});
    alg_4.addTurn({Layer::R, true});
    alg_4.addTurn({Layer::L, true});
    alg_4.addTurn({Layer::L, true});
    alg_4.addTurn({Layer::R, false});
    alg_4.addTurn({Layer::R, true});
    alg_4.addTurn({Layer::F, true});
    alg_4.addTurn({Layer::R, false});
    assert(alg_4.hasHiddenInversion());

    Algorithm alg_5;
    alg_5.addTurn({Layer::R, true});
    alg_5.addTurn({Layer::F, true});
    alg_5.addTurn({Layer::R, false});
    alg_5.addTurn({Layer::R, true});
    alg_5.addTurn({Layer::L, true});
    alg_5.addTurn({Layer::L, false});
    alg_5.addTurn({Layer::R, false});
    alg_5.addTurn({Layer::R, true});
    alg_5.addTurn({Layer::F, true});
    alg_5.addTurn({Layer::R, false});
    assert(alg_5.hasHiddenInversion());

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

void test_hidden_triples() {
    std::cout << "Testing hidden triples... ";
    
    Algorithm alg_1;
    alg_1.addTurn({Layer::B, false});
    alg_1.addTurn({Layer::F, true});
    alg_1.addTurn({Layer::B, true});
    alg_1.addTurn({Layer::B, true});
    assert(!alg_1.hasHiddenTriple());

    Algorithm alg_2;
    alg_2.addTurn({Layer::B, true});
    alg_2.addTurn({Layer::F, true});
    alg_2.addTurn({Layer::B, true});
    alg_2.addTurn({Layer::B, true});
    assert(alg_2.hasHiddenTriple());

    Algorithm alg_3;
    alg_3.addTurn({Layer::B, true});
    alg_3.addTurn({Layer::F, true});
    alg_3.addTurn({Layer::F, true});
    alg_3.addTurn({Layer::B, true});
    alg_3.addTurn({Layer::B, true});
    assert(alg_3.hasHiddenTriple());

    Algorithm alg_4;
    alg_4.addTurn({Layer::B, true});
    alg_4.addTurn({Layer::F, true});
    alg_4.addTurn({Layer::B, true});
    alg_4.addTurn({Layer::F, true});
    alg_4.addTurn({Layer::B, true});
    assert(alg_4.hasHiddenTriple());

    /* 0:1 - X X (Y | Y') X */
    Algorithm alg_5;
    alg_5.addTurn({Layer::U, true});
    alg_5.addTurn({Layer::L, true});
    alg_5.addTurn({Layer::L, true});
    alg_5.addTurn({Layer::R, false});
    alg_5.addTurn({Layer::L, true});
    assert(alg_5.hasHiddenTriple());

    Algorithm alg_6;
    alg_6.addTurn({Layer::U, true});
    alg_6.addTurn({Layer::L, true});
    alg_6.addTurn({Layer::R, true});
    alg_6.addTurn({Layer::R, false});
    alg_6.addTurn({Layer::L, true});
    assert(!alg_6.hasHiddenTriple());

    /* 1:0 - X (Y | Y') X X */
    Algorithm alg_7;
    alg_7.addTurn({Layer::U, true});
    alg_7.addTurn({Layer::R, true});
    alg_7.addTurn({Layer::L, true});
    alg_7.addTurn({Layer::R, true});
    alg_7.addTurn({Layer::R, true});
    assert(alg_7.hasHiddenTriple());

    Algorithm alg_8;
    alg_8.addTurn({Layer::U, true});
    alg_8.addTurn({Layer::R, true});
    alg_8.addTurn({Layer::L, true});
    alg_8.addTurn({Layer::R, false});
    alg_8.addTurn({Layer::R, true});
    assert(!alg_8.hasHiddenTriple());

    /* 0:2 - X X (Y | Y') (Y | Y') X */
    Algorithm alg_9;
    alg_9.addTurn({Layer::U, true});
    alg_9.addTurn({Layer::R, true});
    alg_9.addTurn({Layer::R, true});
    alg_9.addTurn({Layer::L, true});
    alg_9.addTurn({Layer::L, false});
    alg_9.addTurn({Layer::R, true});
    assert(alg_9.hasHiddenTriple());

    Algorithm alg_10;
    alg_10.addTurn({Layer::U, true});
    alg_10.addTurn({Layer::R, true});
    alg_10.addTurn({Layer::R, true});
    alg_10.addTurn({Layer::L, true});
    alg_10.addTurn({Layer::L, false});
    alg_10.addTurn({Layer::R, false});
    assert(!alg_10.hasHiddenTriple());

    /* 1:1 - X (Y | Y') X (Y | Y') X */
    Algorithm alg_11;
    alg_11.addTurn({Layer::U, true});
    alg_11.addTurn({Layer::R, true});
    alg_11.addTurn({Layer::L, true});
    alg_11.addTurn({Layer::R, true});
    alg_11.addTurn({Layer::L, false});
    alg_11.addTurn({Layer::R, true});
    assert(alg_11.hasHiddenTriple());

    Algorithm alg_12;
    alg_12.addTurn({Layer::U, true});
    alg_12.addTurn({Layer::R, true});
    alg_12.addTurn({Layer::L, true});
    alg_12.addTurn({Layer::R, true});
    alg_12.addTurn({Layer::L, false});
    alg_12.addTurn({Layer::R, false});
    assert(!alg_12.hasHiddenTriple());

    /* 2:0 - X (Y | Y') (Y | Y') X X */
    Algorithm alg_13;
    alg_13.addTurn({Layer::U, true});
    alg_13.addTurn({Layer::R, false});
    alg_13.addTurn({Layer::L, false});
    alg_13.addTurn({Layer::L, true});
    alg_13.addTurn({Layer::R, false});
    alg_13.addTurn({Layer::R, false});
    assert(alg_13.hasHiddenTriple());

    Algorithm alg_14;
    alg_14.addTurn({Layer::U, true});
    alg_14.addTurn({Layer::R, true});
    alg_14.addTurn({Layer::L, true});
    alg_14.addTurn({Layer::L, true});
    alg_14.addTurn({Layer::R, false});
    alg_14.addTurn({Layer::R, false});
    assert(!alg_14.hasHiddenTriple());

    /* 1:2 - X (Y | Y') X (Y | Y') (Y | Y') X */
    Algorithm alg_15;
    alg_15.addTurn({Layer::F, true});
    alg_15.addTurn({Layer::D, false});
    alg_15.addTurn({Layer::U, false});
    alg_15.addTurn({Layer::D, false});
    alg_15.addTurn({Layer::U, false});
    alg_15.addTurn({Layer::U, true});
    alg_15.addTurn({Layer::D, false});
    assert(alg_15.hasHiddenTriple());

    Algorithm alg_16;
    alg_16.addTurn({Layer::F, true});
    alg_16.addTurn({Layer::D, true});
    alg_16.addTurn({Layer::U, true});
    alg_16.addTurn({Layer::D, true});
    alg_16.addTurn({Layer::R, false});
    alg_16.addTurn({Layer::U, false});
    alg_16.addTurn({Layer::D, true});
    assert(!alg_16.hasHiddenTriple());

    /* 2:1 - X (Y | Y') (Y | Y') X (Y | Y') X */
    Algorithm alg_17;
    alg_17.addTurn({Layer::F, true});
    alg_17.addTurn({Layer::D, false});
    alg_17.addTurn({Layer::U, false});
    alg_17.addTurn({Layer::U, false});
    alg_17.addTurn({Layer::D, false});
    alg_17.addTurn({Layer::U, true});
    alg_17.addTurn({Layer::D, false});
    assert(alg_17.hasHiddenTriple());

    Algorithm alg_18;
    alg_18.addTurn({Layer::L, true});
    alg_18.addTurn({Layer::D, true});
    alg_18.addTurn({Layer::U, true});
    alg_18.addTurn({Layer::U, true});
    alg_18.addTurn({Layer::D, true});
    alg_18.addTurn({Layer::L, false});
    alg_18.addTurn({Layer::D, true});
    assert(!alg_18.hasHiddenTriple());

    /* 2:2 - X (Y | Y') (Y | Y') X (Y | Y') (Y | Y') X */
    Algorithm alg_19;
    alg_19.addTurn({Layer::U, true});
    alg_19.addTurn({Layer::F, false});
    alg_19.addTurn({Layer::B, false});
    alg_19.addTurn({Layer::B, true});
    alg_19.addTurn({Layer::F, false});
    alg_19.addTurn({Layer::B, true});
    alg_19.addTurn({Layer::B, false});
    alg_19.addTurn({Layer::F, false});
    assert(alg_19.hasHiddenTriple());

    Algorithm alg_20;
    alg_20.addTurn({Layer::R, true});
    alg_20.addTurn({Layer::F, true});
    alg_20.addTurn({Layer::B, false});
    alg_20.addTurn({Layer::B, false});
    alg_20.addTurn({Layer::F, true});
    alg_20.addTurn({Layer::L, false});
    alg_20.addTurn({Layer::B, true});
    alg_20.addTurn({Layer::F, true});
    assert(!alg_20.hasHiddenTriple());

    std::cout << "Passed" << std::endl;
}

void verify_turns(std::vector<Turn> results, std::vector<Turn> expected) {
    assert(results.size() == expected.size());
    for (unsigned int i = 0; i < expected.size(); i++) {
        assert(expected.at(i).layer     == results.at(i).layer);
        assert(expected.at(i).clockwise == results.at(i).clockwise);
    }
}

void test_string() {
    std::cout << "Testing string... ";

    std::string str_1 = "F' L U' R' D B F'";
    std::string str_2;
    Algorithm alg_1;

    alg_1.setAlgorithm(str_1.c_str());
    str_2 = alg_1.getAlgorithmStr();

    assert(str_1.compare(str_2) == 0);

    std::cout << "Passed" << std::endl;
}
