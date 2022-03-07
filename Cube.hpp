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
 *    This class models a traditional cube. The user can select the reference
 *    color, the cube size, and affect turns.
 * 
 *    Rubik's cubes are size three (3x3x3). This class currently simulates
 *    moves for cubes of size two (2x2x2) and three (3x3x3). Larger cubes can
 *    be rendered, but some cubie combinations cannot be achieved because
 *    multi-layer moves are unsupported.
 * 
 * Glossary of Terms:
 *    * Cubie: One discrete sub-cube on the cube. Corner cubies have three
 *      visible faces, edge cubies have two visible faces, and center cubies
 *      have one visible face.
 *    * Edge: The cubies around the outer rim of a layer are known as the
 *      edges. There are four sets of edges on each layer.
 *    * Layer: When you rotate a side, you are rotating a square array of
 *      cubies. This array is known as a "layer". Every visible side is a
 *      layer. In addition there are three internal layers (Middle, Equator,
 *      and Standing). See M, E, and S below for more information on internal
 *      layers.
 *    * Reference: The color of the side known as the "Face" (F). All other
 *      sides will be colored relative to the reference side. There is no
 *      default reference color.
 *    * Size: The number of cubies in any edge is defined as the cube size.
 * 
 * Layer Naming:
 *    F = Face
 *    U = Up
 *    R = Right
 *    D = Down
 *    L = Left
 *    B = Back
 *    M = Middle   (Simulated by turning R and L in the same direction.)
 *    E = Equator  (Simulated by turning U and D in the same direction.)
 *    S = Standing (Simulated by turning F and B in the same direction.)
 * 
 * Internal Cube Model:
 *    The cube is modeled as a two dimensional (MxN) array representing an 
 *    unfolded cube. 
 * 
 *       M = 4*cubeSize
 *       N = 3*cubeSize  
 *
 *    Layers are arranged in the internal model in the following fashion:
 * 
 *       . U . .  
 *       L F R B
 *       . D . .
 * 
 *    Edge coordinates for a 3x3x3 cube. Edge listing is in clockwise order.
 *       (F) UF -  2, 3  2, 4  2, 5; RF -  3, 6  4, 6  5, 6; 
 *           DF -  6, 5  6, 4  6, 3; LF -  5, 2  4, 2  3, 2;
 *       (U) FU -  3, 5  3, 4  3, 3; LU -  3, 2  3, 1  3, 0; 
 *           BU -  3,11  3,10  3, 9; RU -  3, 8  3, 7  3, 6;
 *       (L) UL -  0, 3  1, 3  2, 3; FL -  3, 3  4, 3  5, 3; 
 *           DL -  6, 3  7, 3  8, 3; BL -  5,11  4,11  3,11;
 *       (R) UR -  2, 5  1, 5  0, 5; BR -  3, 9  4, 9  5, 9; 
 *           DR -  8, 5  7, 5  6, 5; FR -  5, 5  4, 5  3, 5;
 *       (D) FD -  5, 3  5, 4  5, 5; RD -  5, 6  5, 7  5, 8;
 *           BD -  5, 9  5,10  5,11; LD -  5, 0  5, 1  5, 2;
 *       (B) UB -  0, 5  0, 4  0, 3; LB -  3, 0  4, 0  5, 0;
 *           DB -  8, 3  8, 4  8, 5; RB -  5, 8  4, 8  3, 8;
 */

#ifndef CUBE_HPP
#define CUBE_HPP

#include <vector>
#include "Algorithm.hpp"

enum CubieColor : char {
    BLUE    = 'b', GREEN  = 'g',
    ORANGE  = 'o', RED    = 'r',
    WHITE   = 'w', YELLOW = 'y',
    NOCOLOR = ' '
};

struct Coordinate {
    unsigned int row;
    unsigned int col;
};

class Cube {
    public:
        Cube();
        Cube(CubieColor referenceColor);
        Cube(CubieColor referenceColor, unsigned int cubeSize);
        Cube(const Cube& obj);
        Cube(Cube&& obj);
        ~Cube();

        Cube& operator=(const Cube& rhs);
        Cube& operator=(Cube&& rhs);
        bool operator==(const Cube& obj);
        bool operator!=(const Cube& obj);

        unsigned int getCubeSize();

        /**
        * The vector returned represents a grid with dimensions
        * cubeSize*4 x cubeSize*3. Essentially a cube flattened out in a 2D
        * plane with whitespace filling in the interstitial fields.
        */
        std::vector<CubieColor> getCube();
      
        static char cubieColorToChar(CubieColor cubie);

        bool isSolved();
        void turn(Turn t);
        void performAlgorithm(const std::vector<Turn> &algorithm);

    private:
        struct Square {
            Coordinate ul; // Upper Left
            Coordinate ur; // Upper Right
            Coordinate lr; // Lower Right
            Coordinate ll; // Lower Left
        };

        enum Edges {
            UpFace=0, RightFace, DownFace,  LeftFace,  // F (face)
            FaceUp,   LeftUp,    BackUp,    RightUp,   // U (up)
            UpLeft,   FaceLeft,  DownLeft,  BackLeft,  // L (left)
            UpRight,  BackRight, DownRight, FaceRight, // R (right)
            FaceDown, RightDown, BackDown,  LeftDown,  // D (down)
            UpBack,   LeftBack,  DownBack,  RightBack  // B (back)
        };

        void destroyCube();
        void copyCube(const Cube& from);
        void copyCubeAttributes(const Cube& from);

        void initializeCube();

        void initializeLayers();
        void initializeLayer(Layer layer, CubieColor color);

        void initializeEdges();
        void initializeFaceEdges();
        void initializeUpEdges();
        void initializeLeftEdges();
        void initializeRightEdges();
        void initializeDownEdges();
        void initializeBackEdges();

        void rotateLayer(Layer layer, bool clockwise);
        void rotateEdges(Edges start, bool clockwise);
        void fourWayRotate(Square square, bool clockwise);

        bool isSolved(Coordinate upperLeft, Coordinate upperLeftMax);
        void getLayerUpperLeft(Coordinate& coord, Layer l);

        CubieColor fInitColor;
        CubieColor uInitColor;
        CubieColor dInitColor;
        CubieColor lInitColor;
        CubieColor rInitColor;
        CubieColor bInitColor;

        unsigned int cubeSize;
        CubieColor** cube;
        Coordinate* edges;

        unsigned int MIN_SIZE       = 2;
        unsigned int DEFAULT_SIZE   = 3;
        unsigned int LAYERS_PER_COL = 3;
        unsigned int LAYERS_PER_ROW = 4;
        unsigned int NUM_EDGE_TYPES = 24;

        /* Cache layer coordinates to speed up turning and solution checking. */
        Coordinate fUpperLeft, fUpperLeftMax;
        Coordinate uUpperLeft, uUpperLeftMax;
        Coordinate lUpperLeft, lUpperLeftMax;
        Coordinate rUpperLeft, rUpperLeftMax;
};

#endif // CUBE_HPP
