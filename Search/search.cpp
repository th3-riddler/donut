#include <iostream>

#include "search.hpp"

int Search::ply;

int Search::killerMoves[2][64];
int Search::historyMoves[12][64];
int Search::pvLength[64];
int Search::pvTable[64][64];
bool Search::followPv;
bool Search::scorePv;


inline void Search::enablePvScore(moves *moveList) {
    followPv = false;

    for (int count = 0; count < moveList->count; count++) {
        if (pvTable[0][ply] == moveList->moves[count]) {
            scorePv = true;
            followPv = true;
        }
    }
}

int Search::quiescenceSearch(int alpha, int beta) {

    Chessboard::nodes++;
    
    int evaluation = Evaluation::evaluate();

    // Fail hard beta-cutoff
    if (evaluation >= beta) {
        return beta;
    }

    if (evaluation > alpha) {
        alpha = evaluation;
    }

    
    moves moveList[1];
    Chessboard::generateMoves(moveList);

    Move::sortMoves(moveList);

    for (int count = 0; count < moveList->count; count++) {
        copyBoard();

        ply++;

        if(Chessboard::makeMove(moveList->moves[count], Chessboard::capturesOnly) == 0) {
            ply--;
            continue;
        }

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

    pvLength[ply] = ply;

    if (depth == 0) {
        return quiescenceSearch(alpha, beta);
    }

    if (ply > maxPly - 1) {
        return Evaluation::evaluate();
    }

    Chessboard::nodes++;

    int inCheck = Chessboard::isSquareAttacked(Chessboard::getLSBIndex((Chessboard::bitboard.sideToMove == Chessboard::white ? Chessboard::bitboard.bitboards[Chessboard::K] : Chessboard::bitboard.bitboards[Chessboard::k])), Chessboard::bitboard.sideToMove ^ 1);

    if (inCheck) {
        depth++;
    }

    int legalMoves = 0;

    moves moveList[1];
    Chessboard::generateMoves(moveList);

    // If i'm following the PV line
    if (followPv) {
        enablePvScore(moveList);
    }

    Move::sortMoves(moveList);

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
            if (getMoveCapture(moveList->moves[count]) == 13) {
                killerMoves[1][ply] = killerMoves[0][ply];
                killerMoves[0][ply] = moveList->moves[count];
            }
            return beta;
        }

        if (score > alpha) {
            if (getMoveCapture(moveList->moves[count]) == 13) {
                historyMoves[getMovePiece(moveList->moves[count])][getMoveTarget(moveList->moves[count])] += depth;
            }

            alpha = score;

            pvTable[ply][ply] = moveList->moves[count];
            for (int nextPly = ply + 1; nextPly < pvLength[ply + 1]; nextPly++) {
                pvTable[ply][nextPly] = pvTable[ply + 1][nextPly]; // Copy the PV line from the next ply to the current ply
            }
            pvLength[ply] = pvLength[ply + 1];
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

    // Node fails low
    return alpha;
}

// Iterative Deepening
int Search::iterativeDeepening(int depth) {
    int score = 0;
    for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {
        Chessboard::nodes = 0;
        followPv = true;

        score = negamax(-50000, 50000, currentDepth);

        std::cout << "info score cp " << score << " depth " << currentDepth << " nodes " << Chessboard::nodes << " pv ";

        for (int count = 0; count < pvLength[0]; count++) {
            Move::printMove(pvTable[0][count]);
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    return score;
}


void Search::searchPosition(int depth) {
    Chessboard::nodes = 0;
    followPv = false;
    scorePv = false;

    memset(killerMoves, 0, sizeof(killerMoves));
    memset(historyMoves, 0, sizeof(historyMoves));
    memset(pvTable, 0, sizeof(pvTable));
    memset(pvLength, 0, sizeof(pvLength));

    int score = iterativeDeepening(depth);



    // ONLY FOR COMPARISON PURPOSES
    Chessboard::nodes = 0;
    followPv = false;
    scorePv = false;

    memset(killerMoves, 0, sizeof(killerMoves));
    memset(historyMoves, 0, sizeof(historyMoves));
    memset(pvTable, 0, sizeof(pvTable));
    memset(pvLength, 0, sizeof(pvLength));

    score = negamax(-50000, 50000, depth);

    std::cout << "info score cp " << score << " depth " << depth << " nodes " << Chessboard::nodes << " pv ";
    for (int count = 0; count < pvLength[0]; count++) {
        Move::printMove(pvTable[0][count]);
        std::cout << " ";
    }
    std::cout << std::endl;

    std::cout << "bestmove ";
    Move::printMove(pvTable[0][0]);
    std::cout << std::endl;
}