#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../Evaluation/evaluation.hpp"
#include "../Chessboard/chessboard.hpp"
#include "../Macros/macros.hpp"
#include "../Move/move.hpp"

class Search {
    public:
        static int negamax(int alpha, int beta, int depth);

        static void searchPosition(int depth);

        static int ply;
        static int bestMove;
};







#endif // SEARCH_HPP