#include <cmath>
#include <iostream>
#include "Cube.h"

Cube::Cube(CubieColor referenceColor) {
   this->cubeSize = DEFAULT_SIZE;
   this->fInitColor = referenceColor;
   initializeCube();
}

Cube::Cube(CubieColor referenceColor, int cubeSize) {
   if (cubeSize < MIN_SIZE)
      cubeSize = MIN_SIZE;

   this->cubeSize = cubeSize;
   this->fInitColor = referenceColor;
   initializeCube();
}

Cube::Cube(const Cube &obj) {
   this->cubeSize = obj.cubeSize;
   this->fInitColor = obj.fInitColor;
   initializeCube();
   copyCube(obj);
}

Cube& Cube::operator=(const Cube &rhs) {
   if (&rhs != this) {
      destroyCube();
      this->cubeSize = rhs.cubeSize;
      this->fInitColor = rhs.fInitColor;
      initializeCube();
      copyCube(rhs);
   }
   return *this;
}

Cube::~Cube() {
   destroyCube();
}

void Cube::destroyCube() {
   for (int i=0; i<(LAYERS_PER_COL*cubeSize); i++)
      delete[] cube[i];
   delete[] cube;
   delete[] edges;
}

void Cube::copyCube(const Cube &from) {
   for (int row=0; row<LAYERS_PER_COL*cubeSize; row++)
      for (int col=0; col<LAYERS_PER_ROW*cubeSize; col++)
         cube[row][col] = from.cube[row][col];
}

void Cube::printCube() {
   for (int row=0; row<LAYERS_PER_COL*cubeSize; row++) {
      for (int col=0; col<LAYERS_PER_ROW*cubeSize; col++) {
         std::cout << cube[row][col] << " ";
      }
      std::cout << std::endl;
   }
}

void Cube::printLayer(Layer layer) {
   Coordinate ul;
   getLayerUpperLeft(ul, layer);

   for (int row=ul.row; row<(ul.row + cubeSize); row++) {
      for (int col=ul.col; col<(ul.col + cubeSize); col++) {
         std::cout << cube[row][col] << " ";
      }
      std::cout << std::endl;
   }
}

void Cube::printEdgeCoordinates() {
   for (int i=0; i<NUM_EDGE_TYPES*cubeSize; i++)
      std::cout << "Edge=" << i << " row=" << edges[i].row 
                                << " col=" << edges[i].col 
                                << std::endl;
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
   for (int row=upperLeft.row; row<upperLeftMax.row; row++)
      for (int col=upperLeft.col; col<upperLeftMax.col; col++)
         if (cubieVal != cube[row][col])
            return false;
   return true;
}

void Cube::turn(Layer layer, bool clockwise) {
   switch (layer) {
      case Layer::F:
         rotateLayer(layer, clockwise);
         rotateEdges(Edges::UpFace, clockwise);
         break;
      case Layer::U:
         rotateLayer(layer, clockwise);
         rotateEdges(Edges::FaceUp, clockwise);
         break;
      case Layer::L:
         rotateLayer(layer, clockwise);
         rotateEdges(Edges::UpLeft, clockwise);
         break;
      case Layer::R:
         rotateLayer(layer, clockwise);
         rotateEdges(Edges::UpRight, clockwise);
         break;
      case Layer::D:
         rotateLayer(layer, clockwise);
         rotateEdges(Edges::FaceDown, clockwise);
         break;
      case Layer::B:
         rotateLayer(layer, clockwise);
         rotateEdges(Edges::UpBack, clockwise);
         break;
      case Layer::M:
         turn(Layer::R, clockwise);
         turn(Layer::L, clockwise);
         break;
      case Layer::E:
         turn(Layer::U, clockwise);
         turn(Layer::D, clockwise);
         break;
      case Layer::S:
         turn(Layer::F, clockwise);
         turn(Layer::B, clockwise);
         break;
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
   int subCubeSize, subLayerMax;
   int ulr, ulc, urr, urc, llr, llc, lrr, lrc;
   Coordinate ul;
   
   getLayerUpperLeft(ul, layer);
   subLayerMax = ceil((float)cubeSize/2);
   
   for (int subLayer=0; subLayer<subLayerMax; subLayer++) {
      subCubeSize = cubeSize - 2*subLayer;
      
      ulr = ul.row + subLayer;
      urc = ul.col + subLayer + subCubeSize - 1;
      llc = ul.col + subLayer;
      lrr = ul.row + subLayer + subCubeSize - 1;
      
      for (int i=0; i<(subCubeSize - 1); i++) {
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
   int index0 = start*cubeSize;
   int index1 = index0 + cubeSize;
   int index2 = index1 + cubeSize;
   int index3 = index2 + cubeSize;
   
   for (int i=0; i<cubeSize; i++)
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
   char tmp = cube[square.ll.row][square.ll.col];
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
   cube = new char*[LAYERS_PER_COL*cubeSize];
   for (int i=0; i<(LAYERS_PER_COL*cubeSize); i++)
      cube[i] = new char[LAYERS_PER_ROW*cubeSize];

   for (int row=0; row<LAYERS_PER_COL*cubeSize; row++)
      for (int col=0; col<LAYERS_PER_ROW*cubeSize; col++)
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
         dInitColor = CubieColor::YELLOW;
         lInitColor = CubieColor::RED;
         rInitColor = CubieColor::ORANGE;
         bInitColor = CubieColor::GREEN;
         break;
      case CubieColor::GREEN:
         uInitColor = CubieColor::WHITE;
         dInitColor = CubieColor::YELLOW;
         lInitColor = CubieColor::ORANGE;
         rInitColor = CubieColor::RED;
         bInitColor = CubieColor::BLUE;
         break;
      case CubieColor::ORANGE:
         uInitColor = CubieColor::WHITE;
         dInitColor = CubieColor::YELLOW;
         lInitColor = CubieColor::BLUE;
         rInitColor = CubieColor::GREEN;
         bInitColor = CubieColor::RED;
         break;
      case CubieColor::RED:
         uInitColor = CubieColor::WHITE;
         dInitColor = CubieColor::YELLOW;
         lInitColor = CubieColor::GREEN;
         rInitColor = CubieColor::BLUE;
         bInitColor = CubieColor::ORANGE;
         break;
      case CubieColor::WHITE:
         uInitColor = CubieColor::GREEN;
         dInitColor = CubieColor::BLUE;
         lInitColor = CubieColor::RED;
         rInitColor = CubieColor::ORANGE;
         bInitColor = CubieColor::YELLOW;
         break;
      case CubieColor::YELLOW:
         uInitColor = CubieColor::GREEN;
         dInitColor = CubieColor::BLUE;
         lInitColor = CubieColor::ORANGE;
         rInitColor = CubieColor::RED;
         bInitColor = CubieColor::WHITE;
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

   for (int r = ul.row; r < (ul.row + cubeSize); r++)
      for (int c = ul.col; c < (ul.col + cubeSize); c++)
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

   for (int i=0; i<cubeSize; i++) {
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

   for (int i=0; i<cubeSize; i++) {
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

   for (int i=0; i<cubeSize; i++) {
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

   for (int i=0; i<cubeSize; i++) {
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

   for (int i=0; i<cubeSize; i++) {
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

   for (int i=0; i<cubeSize; i++) {
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

void Cube::getLayerUpperLeft(Coordinate &coord, Layer l) {
   coord.col = (l%LAYERS_PER_ROW)*cubeSize;
   if (l < LAYERS_PER_ROW)
      coord.row = 0;
   else if (l < LAYERS_PER_ROW*2)
      coord.row = cubeSize;
   else
      coord.row = cubeSize*2;
}
