#include <iostream>
#include "Cube.h"

Cube getScrambled();
void testOperators();
void testLayers();
void testSizes();
void testRotation();

int main() {
   testOperators();
   testLayers();
   testSizes();
   testRotation();

   return 0;
}

void testOperators() {
   std::cout << "Testing copy constructor..." << std::endl;

   Cube c1 = getScrambled();
   Cube c2(c1);
   
   std::cout << "Original..." << std::endl;
   c1.printCube();
   std::cout << std::endl;

   std::cout << "Copy..." << std::endl;
   c2.printCube();
   std::cout << std::endl;

   std::cout << "Assignment..." << std::endl;
   Cube c3 = c2;
   c3.printCube();
   std::cout << std::endl;
}

void testLayers() {
   std::cout << "Testing Layer colors..." << std::endl;

   Cube b(CubieColor::BLUE);
   b.printCube();
   std::cout << std::endl;
   
   Cube g(CubieColor::GREEN);
   g.printCube();
   std::cout << std::endl;
   
   Cube o(CubieColor::ORANGE);
   o.printCube();
   std::cout << std::endl;

   Cube r(CubieColor::RED);
   r.printCube();
   std::cout << std::endl;

   Cube w(CubieColor::WHITE);
   w.printCube();
   std::cout << std::endl;

   Cube y(CubieColor::YELLOW);
   y.printCube();
   std::cout << std::endl;
}

void testSizes() {
   std::cout << "Cube sizes.." << std::endl;

   Cube c3(CubieColor::RED);
   c3.printCube();
   std::cout << std::endl;

   Cube c4(CubieColor::RED, 4);
   c4.printCube();
   std::cout << std::endl;

   Cube c5(CubieColor::RED, 5);
   c5.printCube();
   std::cout << std::endl;

   Cube c6(CubieColor::RED, 6);
   c6.printCube();
   std::cout << std::endl;

   Cube c7(CubieColor::RED, 7);
   c7.printCube();
   std::cout << std::endl;

   Cube c8(CubieColor::RED, 8);
   c8.printCube();
   std::cout << std::endl;

   Cube c9(CubieColor::RED, 9);
   c9.printCube();
   std::cout << std::endl;

   Cube c10(CubieColor::RED, 10);
   c10.printCube();
   std::cout << std::endl;
}

void testRotation() {
   std::cout << "Testing rotation CW and CCW..." << std::endl;

   Cube c(CubieColor::RED, 3);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   std::cout << "Turning Up Layer..." << std::endl;
   c.turn(Layer::U, true);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   c.turn(Layer::U, false);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   std::cout << "Turning Left Layer..." << std::endl;
   c.turn(Layer::L, true);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   c.turn(Layer::L, false);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   std::cout << "Turning Face Layer..." << std::endl;
   c.turn(Layer::F, true);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   c.turn(Layer::F, false);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   std::cout << "Turning Right Layer..." << std::endl;
   c.turn(Layer::R, true);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   c.turn(Layer::R, false);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   std::cout << "Turning Back Layer..." << std::endl;
   c.turn(Layer::B, true);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   c.turn(Layer::B, false);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   std::cout << "Turning Down Layer..." << std::endl;
   c.turn(Layer::D, true);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;

   c.turn(Layer::D, false);
   c.printCube();
   std::cout << "Cube is " << (c.isSolved() ? "solved." : "not solved.") << std::endl;
   std::cout << std::endl;
}

/**
 * Returns a cube in the following arrangement:
 * 
 *      r o o             
 *      b w o             
 *      b y o             
 * g w o w r b y b g w w w 
 * g g w r r b y b y o o y 
 * o o y g g b r g b y b y 
 *      r r w             
 *      g y w             
 *      g r r 
 */
Cube getScrambled() {
   Cube c(CubieColor::RED, 3);
   c.turn(Layer::R, false);
   c.turn(Layer::B, true);
   c.turn(Layer::U, false);
   c.turn(Layer::L, true);
   c.turn(Layer::F, false);
   c.turn(Layer::U, true);
   c.turn(Layer::F, false);
   c.turn(Layer::D, true);
   return c;
}
