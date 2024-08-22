#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../Evaluation/evaluation.hpp"
#include "../Chessboard/chessboard.hpp"
#include "../Macros/macros.hpp"
#include "../Move/move.hpp"

class Search {
    public:
        static int negamax(int alpha, int beta, int depth);
        static int quiescenceSearch(int alpha, int beta);

        static void searchPosition(int depth);

        static int ply;

        static int killerMoves[2][64];
        static int historyMoves[12][64];
        static int pvLength[64];
        static int pvTable[64][64];
};







#endif // SEARCH_HPP