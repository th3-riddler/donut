#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include <cstdint>

#include "../Chessboard/chessboard.hpp"

class Evaluation {
    public:
        static const int materialScore[12];
        static const int pawnScore[64];
        static const int knightScore[64];
        static const int bishopScore[64];
        static const int rookScore[64];
        static const int queenScore[64];
        static const int kingScore[64];
        static const int mirrorPieceScore[128];
        static const int mvvLva[12][12];

        static uint64_t fileMasks[64];
        static uint64_t rankMasks[64];
        static uint64_t isolatedMasks[64];
        static uint64_t whitePassedMasks[64];
        static uint64_t blackPassedMasks[64];

        static int evaluate();
        static int scoreMove(int move);
        static uint64_t setFileRankMask(int fileNumber, int rankNumber);
        static void initEvalMasks();
};




#endif // EVALUATION_HPP