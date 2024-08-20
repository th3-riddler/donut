#include <iostream>

#include "search.hpp"

int Search::ply;
int Search::bestMove;


int Search::quiescenceSearch(int alpha, int beta) {
    
    int evaluation = Evaluation::evaluate();

    // std::cout << "Alpha: " << alpha << std::endl;
    // std::cout << "Beta: " << beta << std::endl;
    // std::cout << "Evaluation: " << evaluation << std::endl;    
    // std::cout << std::endl;

    // Fail hard beta-cutoff
    if (evaluation >= beta) { // 0 -5 0 -5 0 -10
        return beta;
    }

    if (evaluation > alpha) {
        alpha = evaluation;
    }

    
    moves moveList[1];
    Chessboard::generateMoves(moveList);

    for (int count = 0; count < moveList->count; count++) {
        copyBoard();

        ply++;

        if(Chessboard::makeMove(moveList->moves[count], Chessboard::capturesOnly) == 0) {
            ply--;
            continue;
        }

        std::cout << "Quiescence Move: ";
        Chessboard::printMove(moveList->moves[count]);
        std::cout << std::endl; 

        int score = -quiescenceSearch(-beta, -alpha);

        ply--;

        takeBack();

        // Fail hard beta-cutoff
        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

int Search::negamax(int alpha, int beta, int depth) { // position startpos moves d2d4 d7d5 e2e4 d5e4 b1c3
    if (depth == 0) {
        return quiescenceSearch(alpha, beta);
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
            return -49000 + ply;
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
    std::cout << "Depth: " << depth << std::endl;
    int score = negamax(-50000, 50000, depth);

    if (bestMove) {
        std::cout << "info score cp " << score << " depth " << depth << " nodes " << Chessboard::nodes << std::endl;

        std::cout << "bestmove ";
        Chessboard::printMove(bestMove);
        std::cout << std::endl;
    }
}