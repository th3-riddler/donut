#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../Evaluation/evaluation.hpp"
#include "../Chessboard/chessboard.hpp"
#include "../Macros/macros.hpp"
#include "../Move/move.hpp"

struct moves;

class Search {
    public:
        static int negamax(int alpha, int beta, int depth);
        static int quiescenceSearch(int alpha, int beta);
        static int iterativeDeepening(int depth);

        static void searchPosition(int depth);
        static inline void enablePvScore(moves *moveList);

        static int ply;

        static int killerMoves[2][maxPly];
        static int historyMoves[12][64];
        static int pvLength[maxPly];
        static int pvTable[maxPly][maxPly];
        static bool followPv;
        static bool scorePv;
};







#endif // SEARCH_HPP