#include <iostream>

#include "search.hpp"

int Search::ply;
int Search::bestMove;

int Search::negamax(int alpha, int beta, int depth) {
    if (depth == 0) {
        return Evaluation::evaluate();
    }

    Chessboard::nodes++;

    int inCheck = Chessboard::isSquareAttacked(Chessboard::getLSBIndex((Chessboard::bitboard.sideToMove == Chessboard::white ? Chessboard::bitboard.bitboards[Chessboard::K] : Chessboard::bitboard.bitboards[Chessboard::k])), Chessboard::bitboard.sideToMove ^ 1);
    int legalMoves = 0;

    int bestSoFar;
    int oldAlpha = alpha;

    moves moveList[1];
    Chessboard::generateMoves(moveList);

    for (int count = 0; count < moveList->count; count++) {
        copyBoard();

        ply++;

        if(Chessboard::makeMove(moveList->moves[count], Chessboard::allMoves) == 0) {
            ply--;
            continue;
        }

        legalMoves++;

        int score = -negamax(-beta, -alpha, depth - 1);

        ply--;

        takeBack();

        // Fail hard beta-cutoff
        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
            if (ply == 0) {
                bestSoFar = moveList->moves[count];
            }
        }
    }

    if (legalMoves == 0) {
        if (inCheck) {
            return -9999 + ply;
        }
        else {
            return 0;
        }
    }

    if (oldAlpha != alpha) {
        bestMove = bestSoFar;
    }

    // Node fails low
    return alpha;
}

void Search::searchPosition(int depth) {
    int score = negamax(-999999, 999999, depth);

    if (bestMove) {
        std::cout << "info score cp " << score << " depth " << depth << " nodes " << Chessboard::nodes << std::endl;

        std::cout << "bestmove ";
        Chessboard::printMove(bestMove);
        std::cout << std::endl;
    }
}