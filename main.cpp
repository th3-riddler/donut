#include "game.hpp"
#include "move.hpp"
#include "chessboard.hpp"

int main(int argc, char* argv[]) {
    Move::init();
    Chessboard::init();
    return 0;
}