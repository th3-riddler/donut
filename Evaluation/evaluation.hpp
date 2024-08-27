#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include <cstdint>

#include "../Chessboard/chessboard.hpp"

class Evaluation {
    public:
        static const int materialScore[2][12];
        static const int openingPhaseScore;
        static const int endgamePhaseScore;
        static const int positionalScore[2][6][64];

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
        static const int getRank[64];
        static const int doublePawnPenalty;
        static const int isolatedPawnPenalty;
        static const int passedPawnBonus[8];
        static const int semiOpenFileBonus;
        static const int openFileBonus;
        static const int kingShieldBonus;

        static int evaluate();
        static int scoreMove(int move);
        static uint64_t setFileRankMask(int fileNumber, int rankNumber);
        static void initEvalMasks();
        static inline int getGamePhaseScore();

        enum { opening, endgame, middlegame };
        enum { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
};




#endif // EVALUATION_HPP