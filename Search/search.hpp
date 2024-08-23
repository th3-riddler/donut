#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../Evaluation/evaluation.hpp"
#include "../Chessboard/chessboard.hpp"
#include "../Macros/macros.hpp"
#include "../Move/move.hpp"

struct moves;

struct tt {
    uint64_t hashKey;
    int depth;
    int flags;
    int score;
};

class Search {
    public:
        static int negamax(int alpha, int beta, int depth);
        static int quiescenceSearch(int alpha, int beta);

        static void searchPosition(int depth);
        static inline void enablePvScore(moves *moveList);
        static void clearTranspositionTable();

        static int ply;

        static int killerMoves[2][maxPly];
        static int historyMoves[12][64];
        static int pvLength[maxPly];
        static int pvTable[maxPly][maxPly];
        static bool followPv;
        static bool scorePv;

        static const int fullDepthMoves;
        static const int reductionLimit;
        static tt transpositionTable[hashSize];
};







#endif // SEARCH_HPP