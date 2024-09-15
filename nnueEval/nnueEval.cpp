#include "../nnue/probe.h"
#include "nnueEval.hpp"

void init_nnue(const char *bigNetName, const char *smallNetName) {
    Stockfish::Probe::init(bigNetName, smallNetName);
}

int evaluate_nnue(const int pieces[], const int squares[], int pieceAmount, bool side, int rule50) {
    return Stockfish::Probe::eval(pieces, squares, pieceAmount, side, rule50);
}

// int evaluate_fen_nnue(char *fen) {
//     return nnue_evaluate_fen(fen);
// }