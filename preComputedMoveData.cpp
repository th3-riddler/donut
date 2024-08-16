#include "preComputedMoveData.hpp"
#include <math.h>


const int preComputedMoveData::directionOffsets[] = { 8, -8, -1, 1, 7, -7, 9, -9 };
int preComputedMoveData::numSquaresToEdge[8][8];

void preComputedMoveData::initialize() {
    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
            int y = squareIndex / 8;
            int x = squareIndex - y * 8;

            int north = 7 - y;
            int south = y;
            int west = x;
            int east = 7 - x;
            preComputedMoveData::numSquaresToEdge[squareIndex][0] = north;
            preComputedMoveData::numSquaresToEdge[squareIndex][1] = south;
            preComputedMoveData::numSquaresToEdge[squareIndex][2] = west;
            preComputedMoveData::numSquaresToEdge[squareIndex][3] = east;
            preComputedMoveData::numSquaresToEdge[squareIndex][4] = std::min(north, west);
            preComputedMoveData::numSquaresToEdge[squareIndex][5] = std::min(south, east);
            preComputedMoveData::numSquaresToEdge[squareIndex][6] = std::min(north, east);
            preComputedMoveData::numSquaresToEdge[squareIndex][7] = std::min(south, west);
        }
}

// void cleanupNumSquaresToEdge() {
//     for (int squareIndex = 0; squareIndex < 64; ++squareIndex) {
//         delete[] preComputedMoveData::numSquaresToEdge[squareIndex];
//     }
// }