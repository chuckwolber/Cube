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

#include <cmath>
#include <vector>
#include "Cube.h"

Cube::Cube() {
   this->cubeSize = DEFAULT_SIZE;
   this->fInitColor = CubieColor::WHITE;
   initializeCube();
}

Cube::Cube(CubieColor referenceColor) {
   this->cubeSize = DEFAULT_SIZE;
   this->fInitColor = referenceColor;
   initializeCube();
}

Cube::Cube(CubieColor referenceColor, unsigned int cubeSize) {
   if (cubeSize < MIN_SIZE)
      cubeSize = MIN_SIZE;

   this->cubeSize = cubeSize;
   this->fInitColor = referenceColor;
   initializeCube();
}

Cube::Cube(const Cube& obj) {
   cubeSize = obj.cubeSize;
   fInitColor = obj.fInitColor;
   initializeCube();
   copyCube(obj);
}

Cube::Cube(Cube&& obj) {
   copyCubeAttributes(obj);
   obj.cube = nullptr;
   obj.edges = nullptr;
}

Cube& Cube::operator=(const Cube& rhs) {
   if (&rhs != this) {
      destroyCube();
      cubeSize = rhs.cubeSize;
      fInitColor = rhs.fInitColor;
      initializeCube();
      copyCube(rhs);
   }
   return *this;
}

Cube& Cube::operator=(Cube&& rhs) {
   if (&rhs != this) {
      destroyCube();
      copyCubeAttributes(rhs);
      rhs.cube = nullptr;
      rhs.edges = nullptr;
   }
   return *this;
}

bool Cube::operator==(const Cube& obj) {
   if (cubeSize != obj.cubeSize)
      return false;

   for (unsigned int row=0; row<LAYERS_PER_COL*cubeSize; row++)
      for (unsigned int col=0; col<LAYERS_PER_ROW*cubeSize; col++)
         if (cube[row][col] != obj.cube[row][col])
            return false;
   return true;
}

bool Cube::operator!=(const Cube& obj) {
   return !(*this == obj);
}

Cube::~Cube() {
   destroyCube();
}

void Cube::destroyCube() {
   if (cube != nullptr) {
      for (unsigned int i=0; i<(LAYERS_PER_COL*cubeSize); i++)
         delete[] cube[i];
      delete[] cube;
      cube = nullptr;
   }

   if (edges != nullptr) {
      delete[] edges;
      edges = nullptr;
   }
}

/* Copy semantics helper. */
void Cube::copyCube(const Cube& from) {
   for (unsigned int row=0; row<LAYERS_PER_COL*cubeSize; row++)
      for (unsigned int col=0; col<LAYERS_PER_ROW*cubeSize; col++)
         cube[row][col] = from.cube[row][col];
}

/* Move semantics helper. */
void Cube::copyCubeAttributes(const Cube& from) {
   cubeSize   = from.cubeSize;
   fInitColor = from.fInitColor;
   cube       = from.cube;
   edges      = from.edges;
   fInitColor = from.fInitColor;
   uInitColor = from.uInitColor;
   dInitColor = from.dInitColor;
   lInitColor = from.lInitColor;
   rInitColor = from.rInitColor;
   bInitColor = from.bInitColor;
   fUpperLeft = from.fUpperLeft;
   uUpperLeft = from.uUpperLeft;
   lUpperLeft = from.lUpperLeft;
   rUpperLeft = from.rUpperLeft;
   fUpperLeftMax = from.fUpperLeftMax;
   uUpperLeftMax = from.uUpperLeftMax;
   lUpperLeftMax = from.lUpperLeftMax;
   rUpperLeftMax = from.rUpperLeftMax;
}

unsigned int Cube::getCubeSize() {
   return cubeSize;
}

std::vector<CubieColor> Cube::getCube() {
   std::vector<CubieColor> tmp;

   for (unsigned int row=0; row<LAYERS_PER_COL*cubeSize; row++)
      for (unsigned int col=0; col<LAYERS_PER_ROW*cubeSize; col++)
         tmp.push_back(cube[row][col]);

   return tmp;
}

char Cube::cubieColorToChar(CubieColor cubie) {
   switch (cubie) {
      case CubieColor::BLUE:
         return 'b';
         break;
      case CubieColor::GREEN:
         return 'g';
         break;
      case CubieColor::ORANGE:
         return 'o';
         break;
      case CubieColor::RED:
         return 'r';
         break;
      case CubieColor::WHITE:
         return 'w';
         break;
      case CubieColor::YELLOW:
         return 'y';
         break;
      default:
         return ' ';
   }
}

char Cube::layerToChar(Layer layer) {
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

Layer Cube::charToLayer(char lChar) {
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

/**
 * https://puzzling.stackexchange.com/questions/86916/minimum-effort-to-detect-a-solved-rubiks-cube
 * 
 * "Any four solved faces is sufficient to prove the entire cube is solved."
 */
bool Cube::isSolved() {
   if (!isSolved(fUpperLeft, fUpperLeftMax))
      return false;
   if (!isSolved(uUpperLeft, uUpperLeftMax))
      return false;
   if (!isSolved(lUpperLeft, lUpperLeftMax))
      return false;
   if (!isSolved(rUpperLeft, rUpperLeftMax))
      return false;
   return true;
}

bool Cube::isSolved(Coordinate upperLeft, Coordinate upperLeftMax) {
   char cubieVal = cube[upperLeft.row][upperLeft.col];
   for (unsigned int row=upperLeft.row; row<upperLeftMax.row; row++)
      for (unsigned int col=upperLeft.col; col<upperLeftMax.col; col++)
         if (cubieVal != cube[row][col])
            return false;
   return true;
}

void Cube::performAlgorithm(const std::vector<Turn> &algorithm) {
   for (const Turn &t : algorithm)
      turn(t);
}

void Cube::turn(Turn t) {
   switch (t.layer) {
      case Layer::F:
         rotateLayer(t.layer, t.clockwise);
         rotateEdges(Edges::UpFace, t.clockwise);
         break;
      case Layer::U:
         rotateLayer(t.layer, t.clockwise);
         rotateEdges(Edges::FaceUp, t.clockwise);
         break;
      case Layer::R:
         rotateLayer(t.layer, t.clockwise);
         rotateEdges(Edges::UpRight, t.clockwise);
         break;
      case Layer::D:
         rotateLayer(t.layer, t.clockwise);
         rotateEdges(Edges::FaceDown, t.clockwise);
         break;
      case Layer::L:
         rotateLayer(t.layer, t.clockwise);
         rotateEdges(Edges::UpLeft, t.clockwise);
         break;
      case Layer::B:
         rotateLayer(t.layer, t.clockwise);
         rotateEdges(Edges::UpBack, t.clockwise);
         break;
      case Layer::M:
         turn({Layer::R, t.clockwise});
         turn({Layer::L, !t.clockwise});
         break;
      case Layer::E:
         turn({Layer::U, t.clockwise});
         turn({Layer::D, !t.clockwise});
         break;
      case Layer::S:
         turn({Layer::F, !t.clockwise});
         turn({Layer::B, t.clockwise});
         break;
      case Layer::NOLAYER:
      default:
         break;
   }
}

/**
 * The atomic element of a layer rotation is a four way circular cubie swap. The
 * four way swap is iterated over the outer edge of the layer. Then the sublayer 
 * is reduced by one in each dimension (row and column) to generate a new 
 * sublayer that needs its outline four way swapped. This continues until we 
 * reach the middle of the layer.
 */
void Cube::rotateLayer(Layer layer, bool clockwise) {
   unsigned int subCubeSize, subLayerMax;
   unsigned int ulr, ulc, urr, urc, llr, llc, lrr, lrc;
   Coordinate ul;
   
   getLayerUpperLeft(ul, layer);
   subLayerMax = ceil((float)cubeSize/2);
   
   for (unsigned int subLayer=0; subLayer<subLayerMax; subLayer++) {
      subCubeSize = cubeSize - 2*subLayer;
      
      ulr = ul.row + subLayer;
      urc = ul.col + subLayer + subCubeSize - 1;
      llc = ul.col + subLayer;
      lrr = ul.row + subLayer + subCubeSize - 1;
      
      for (unsigned int i=0; i<(subCubeSize - 1); i++) {
         ulc = ul.col + subLayer + i;
         urr = ul.row + subLayer + i;
         llr = ul.row + subLayer + subCubeSize - 1 - i;
         lrc = ul.col + subLayer + subCubeSize - 1 - i;
         
         fourWayRotate({{ulr, ulc},  // Upper Left
                        {urr, urc},  // Upper Right
                        {lrr, lrc},  // Lower Right
                        {llr, llc}}, // Lower Left
                       clockwise);
      }
   }
}

void Cube::rotateEdges(Edges start, bool clockwise) {
   unsigned int index0 = start*cubeSize;
   unsigned int index1 = index0 + cubeSize;
   unsigned int index2 = index1 + cubeSize;
   unsigned int index3 = index2 + cubeSize;
   
   for (unsigned int i=0; i<cubeSize; i++)
      fourWayRotate({{edges[index0 + i].row, edges[index0 + i].col},
                     {edges[index1 + i].row, edges[index1 + i].col},
                     {edges[index2 + i].row, edges[index2 + i].col},
                     {edges[index3 + i].row, edges[index3 + i].col}},
                    clockwise);
}

/**
 * Clockwise:
 *    * Move lower right to lower left.
 *    * Move upper right to lower right.
 *    * Move upper left to upper right.
 *    * Restore color to upper left.
 * 
 * Counter-Clockwise:
 *    * Move upper left to lower left.
 *    * Move upper right to upper left.
 *    * Move lower right to upper right.
 *    * Restore color to lower right.
 */
void Cube::fourWayRotate(Square square, bool clockwise) {
   CubieColor tmp = cube[square.ll.row][square.ll.col];
   if (clockwise) {
      cube[square.ll.row][square.ll.col] = cube[square.lr.row][square.lr.col];
      cube[square.lr.row][square.lr.col] = cube[square.ur.row][square.ur.col];
      cube[square.ur.row][square.ur.col] = cube[square.ul.row][square.ul.col];
      cube[square.ul.row][square.ul.col] = tmp;
   } else {
      cube[square.ll.row][square.ll.col] = cube[square.ul.row][square.ul.col];
      cube[square.ul.row][square.ul.col] = cube[square.ur.row][square.ur.col];
      cube[square.ur.row][square.ur.col] = cube[square.lr.row][square.lr.col];
      cube[square.lr.row][square.lr.col] = tmp;
   }
}

void Cube::initializeCube() {
   cube = new CubieColor*[LAYERS_PER_COL*cubeSize];
   for (unsigned int i=0; i<(LAYERS_PER_COL*cubeSize); i++)
      cube[i] = new CubieColor[LAYERS_PER_ROW*cubeSize];

   for (unsigned int row=0; row<LAYERS_PER_COL*cubeSize; row++)
      for (unsigned int col=0; col<LAYERS_PER_ROW*cubeSize; col++)
         cube[row][col] = CubieColor::NOCOLOR;
   
   initializeLayers();
   initializeEdges();

   getLayerUpperLeft(fUpperLeft, Layer::F);
   fUpperLeftMax.row = fUpperLeft.row + cubeSize;
   fUpperLeftMax.col = fUpperLeft.col + cubeSize;

   getLayerUpperLeft(uUpperLeft, Layer::U);
   uUpperLeftMax.row = uUpperLeft.row + cubeSize;
   uUpperLeftMax.col = uUpperLeft.col + cubeSize;

   getLayerUpperLeft(lUpperLeft, Layer::L);
   lUpperLeftMax.row = lUpperLeft.row + cubeSize;
   lUpperLeftMax.col = lUpperLeft.col + cubeSize;

   getLayerUpperLeft(rUpperLeft, Layer::R);
   rUpperLeftMax.row = rUpperLeft.row + cubeSize;
   rUpperLeftMax.col = rUpperLeft.col + cubeSize;
}

void Cube::initializeLayers() {
   switch (fInitColor) {
      case CubieColor::BLUE:
         uInitColor = CubieColor::WHITE;
         lInitColor = CubieColor::RED;
         rInitColor = CubieColor::ORANGE;
         bInitColor = CubieColor::GREEN;
         dInitColor = CubieColor::YELLOW;
         break;
      case CubieColor::GREEN:
         uInitColor = CubieColor::WHITE;
         lInitColor = CubieColor::ORANGE;
         rInitColor = CubieColor::RED;
         bInitColor = CubieColor::BLUE;
         dInitColor = CubieColor::YELLOW;
         break;
      case CubieColor::ORANGE:
         uInitColor = CubieColor::WHITE;
         lInitColor = CubieColor::BLUE;
         rInitColor = CubieColor::GREEN;
         bInitColor = CubieColor::RED;
         dInitColor = CubieColor::YELLOW;
         break;
      case CubieColor::RED:
         uInitColor = CubieColor::WHITE;
         lInitColor = CubieColor::GREEN;
         rInitColor = CubieColor::BLUE;
         bInitColor = CubieColor::ORANGE;
         dInitColor = CubieColor::YELLOW;
         break;
      case CubieColor::WHITE:
         uInitColor = CubieColor::GREEN;
         lInitColor = CubieColor::RED;
         rInitColor = CubieColor::ORANGE;
         bInitColor = CubieColor::YELLOW;
         dInitColor = CubieColor::BLUE;
         break;
      case CubieColor::YELLOW:
         uInitColor = CubieColor::GREEN;
         lInitColor = CubieColor::ORANGE;
         rInitColor = CubieColor::RED;
         bInitColor = CubieColor::WHITE;
         dInitColor = CubieColor::BLUE;
         break;
      default:
         break;
   }

   initializeLayer(Layer::U, uInitColor);
   initializeLayer(Layer::L, lInitColor);
   initializeLayer(Layer::F, fInitColor);
   initializeLayer(Layer::R, rInitColor);
   initializeLayer(Layer::B, bInitColor);
   initializeLayer(Layer::D, dInitColor);
}

void Cube::initializeLayer(Layer layer, CubieColor color) {
   Coordinate ul;
   getLayerUpperLeft(ul, layer);

   for (unsigned int r = ul.row; r < (ul.row + cubeSize); r++)
      for (unsigned int c = ul.col; c < (ul.col + cubeSize); c++)
         cube[r][c] = color;
}

/**
 * Edges are filled into the edges array in a clockwise fashion relevant to 
 * the layer that owns those edges. This mimicks standardized turn logic where
 * "clockwise" is relative to the layer you are turning.
 */
void Cube::initializeEdges() {
   edges = new Coordinate[NUM_EDGE_TYPES*cubeSize];

   initializeFaceEdges();
   initializeUpEdges();
   initializeLeftEdges();
   initializeRightEdges();
   initializeDownEdges();
   initializeBackEdges();
}

void Cube::initializeFaceEdges() {
   Coordinate ul, *c1, *c2, *c3, *c4;
   getLayerUpperLeft(ul, Layer::F);

   for (unsigned int i=0; i<cubeSize; i++) {
      c1 = new Coordinate;
      c1->row = ul.row - 1;
      c1->col = ul.col + i;
      edges[i + Edges::UpFace*cubeSize] = *c1;

      c2 = new Coordinate;
      c2->row = ul.row + i;
      c2->col = ul.col + cubeSize;
      edges[i + Edges::RightFace*cubeSize] = *c2;

      c3 = new Coordinate;
      c3->row = ul.row + cubeSize;
      c3->col = ul.col + cubeSize - 1 - i;
      edges[i + Edges::DownFace*cubeSize] = *c3;

      c4 = new Coordinate;
      c4->row = ul.row + cubeSize - 1 - i;
      c4->col = ul.col - 1;
      edges[i + Edges::LeftFace*cubeSize] = *c4;
   }
}

void Cube::initializeUpEdges() {
   Coordinate ul, *c1, *c2, *c3, *c4;
   getLayerUpperLeft(ul, Layer::U);

   for (unsigned int i=0; i<cubeSize; i++) {
      c1 = new Coordinate;
      c1->row = cubeSize;
      c1->col = ul.col + cubeSize - 1 - i;
      edges[i + Edges::FaceUp*cubeSize] = *c1;

      c2 = new Coordinate;
      c2->row = cubeSize;
      c2->col = ul.col - 1 - i;
      edges[i + Edges::LeftUp*cubeSize] = *c2;

      c3 = new Coordinate;
      c3->row = cubeSize;
      c3->col = ul.col + cubeSize*3 - 1 - i;
      edges[i + Edges::BackUp*cubeSize] = *c3;

      c4 = new Coordinate;
      c4->row = cubeSize;
      c4->col = ul.col + cubeSize*2 - 1 - i;
      edges[i + Edges::RightUp*cubeSize] = *c4;
   }
}

void Cube::initializeLeftEdges() {
   Coordinate ul, *c1, *c2, *c3, *c4;
   getLayerUpperLeft(ul, Layer::L);

   for (unsigned int i=0; i<cubeSize; i++) {
      c1 = new Coordinate;
      c1->row = i;
      c1->col = cubeSize;
      edges[i + Edges::UpLeft*cubeSize] = *c1;

      c2 = new Coordinate;
      c2->row = ul.row + i;
      c2->col = cubeSize;
      edges[i + Edges::FaceLeft*cubeSize] = *c2;

      c3 = new Coordinate;
      c3->row = ul.row + cubeSize + i;
      c3->col = cubeSize;
      edges[i + Edges::DownLeft*cubeSize] = *c3;

      c4 = new Coordinate;
      c4->row = ul.row + cubeSize - 1 - i;
      c4->col = cubeSize*4 - 1;
      edges[i + Edges::BackLeft*cubeSize] = *c4;
   }
}

void Cube::initializeRightEdges() {
   Coordinate ul, *c1, *c2, *c3, *c4;
   getLayerUpperLeft(ul, Layer::R);

   for (unsigned int i=0; i<cubeSize; i++) {
      c1 = new Coordinate;
      c1->row = ul.row - 1 - i;
      c1->col = ul.col - 1;
      edges[i + Edges::UpRight*cubeSize] = *c1;

      c2 = new Coordinate;
      c2->row = ul.row + i;
      c2->col = ul.col + cubeSize;
      edges[i + Edges::BackRight*cubeSize] = *c2;

      c3 = new Coordinate;
      c3->row = ul.row + cubeSize*2 - 1 - i;
      c3->col = ul.col - 1;
      edges[i + Edges::DownRight*cubeSize] = *c3;

      c4 = new Coordinate;
      c4->row = ul.row + cubeSize - 1 - i;
      c4->col = ul.col - 1;
      edges[i + Edges::FaceRight*cubeSize] = *c4;
   }
}

void Cube::initializeDownEdges() {
   Coordinate ul, *c1, *c2, *c3, *c4;
   getLayerUpperLeft(ul, Layer::D);

   for (unsigned int i=0; i<cubeSize; i++) {
      c1 = new Coordinate;
      c1->row = ul.row - 1;
      c1->col = ul.col + i;
      edges[i + Edges::FaceDown*cubeSize] = *c1;

      c2 = new Coordinate;
      c2->row = ul.row - 1;
      c2->col = ul.col + cubeSize + i;
      edges[i + Edges::RightDown*cubeSize] = *c2;

      c3 = new Coordinate;
      c3->row = ul.row - 1;
      c3->col = ul.col + cubeSize*2 + i;
      edges[i + Edges::BackDown*cubeSize] = *c3;

      c4 = new Coordinate;
      c4->row = ul.row - 1;
      c4->col = i;
      edges[i + Edges::LeftDown*cubeSize] = *c4;
   }
}

void Cube::initializeBackEdges() {
   Coordinate ul, *c1, *c2, *c3, *c4;
   getLayerUpperLeft(ul, Layer::B);

   for (unsigned int i=0; i<cubeSize; i++) {
      c1 = new Coordinate;
      c1->row = 0;
      c1->col = cubeSize*2 - 1 - i;
      edges[i + Edges::UpBack*cubeSize] = *c1;

      c2 = new Coordinate;
      c2->row = cubeSize + i;
      c2->col = 0;
      edges[i + Edges::LeftBack*cubeSize] = *c2;

      c3 = new Coordinate;
      c3->row = cubeSize*3 - 1;
      c3->col = cubeSize + i;
      edges[i + Edges::DownBack*cubeSize] = *c3;

      c4 = new Coordinate;
      c4->row = ul.row + cubeSize - 1 - i;
      c4->col = ul.col - 1;
      edges[i + Edges::RightBack*cubeSize] = *c4;
   }
}

void Cube::getLayerUpperLeft(Coordinate& coord, Layer l) {
   /* Layers that are not (yet) supported. */
   if (l == Layer::M || l == Layer::E || l == Layer::S)
      return;

   if (l == Layer::NOLAYER)
      return;

   coord.col = (l%LAYERS_PER_ROW)*cubeSize;
   if (l < LAYERS_PER_ROW)
      coord.row = 0;
   else if (l < LAYERS_PER_ROW*2)
      coord.row = cubeSize;
   else
      coord.row = cubeSize*2;
}
