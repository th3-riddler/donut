#include "Move/move.hpp"
#include "Chessboard/chessboard.hpp"

int main(int argc, char* argv[]) {
    Move::init();
    Chessboard::init();

    free(Search::transpositionTable);
    return 0;
}