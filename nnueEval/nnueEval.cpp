#include "../nnue/nnue.h"
#include "nnueEval.hpp"

void init_nnue(char *filename) {
    nnue_init(filename);
}

int evaluate_nnue(int player, int *pieces, int *squares) {
    return nnue_evaluate(player, pieces, squares);
}

int evaluate_fen_nnue(char *fen) {
    return nnue_evaluate_fen(fen);
}