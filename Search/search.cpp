#include <iostream>

#include "search.hpp"

int Search::ply;

int Search::killerMoves[2][64];
int Search::historyMoves[12][64];
int Search::pvLength[64];
int Search::pvTable[64][64];
bool Search::followPv;
bool Search::scorePv;
const int Search::fullDepthMoves = 4;
const int Search::reductionLimit = 3;
tt Search::transpositionTable[hashSize];


void Search::clearTranspositionTable() {
    for (int index = 0; index < hashSize; index++) {
        transpositionTable[index].hashKey = 0;
        transpositionTable[index].depth = 0;
        transpositionTable[index].flags = 0;
        transpositionTable[index].score = 0;
    }
}

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

    if ((Chessboard::nodes & 2047) == 0) {
        Chessboard::communicate();
    }

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

int Search::negamax(int alpha, int beta, int depth) {

    if ((Chessboard::nodes & 2047) == 0) {
        Chessboard::communicate();
    }

    pvLength[ply] = ply;

    int score = 0;

    if (depth == 0) {
        return quiescenceSearch(alpha, beta);
    }

    if (ply > maxPly - 1) {
        return Evaluation::evaluate();
    }

    Chessboard::nodes++;

    int inCheck = Chessboard::isSquareAttacked(Chessboard::getLSBIndex((Chessboard::bitboard.sideToMove == Chessboard::white ? 
                                                                        Chessboard::bitboard.bitboards[Chessboard::K] : Chessboard::bitboard.bitboards[Chessboard::k])), 
                                                                        Chessboard::bitboard.sideToMove ^ 1);

    if (inCheck) {
        depth++;
    }

    int legalMoves = 0;

    // NULL MOVE PRUNING
    if (depth >= 3 && !inCheck && ply) {
        copyBoard();

        Chessboard::bitboard.sideToMove ^= 1;

        Chessboard::bitboard.enPassantSquare = Chessboard::noSquare;

        // Search Moves with a reduced depth (depth - 1 - R) --> R is the reduction amount
        score = -negamax(-beta, -beta + 1, depth - 1 - 2);

        takeBack();

        if (Chessboard::stopped) {
            return 0;
        }

        if (score >= beta) {
            return beta;
        }
    }

    moves moveList[1];
    Chessboard::generateMoves(moveList);

    if (followPv) {
        enablePvScore(moveList);
    }

    Move::sortMoves(moveList);

    int movesSearched = 0;

    for (int count = 0; count < moveList->count; count++) {
        copyBoard();

        ply++;

        if(Chessboard::makeMove(moveList->moves[count], Chessboard::allMoves) == 0) {
            ply--;
            continue;
        }

        legalMoves++;

        if (movesSearched == 0) {
            score = -negamax(-beta, -alpha, depth - 1);
        }
        else {
            // Late Move Reduction (LMR)
            if (movesSearched >= fullDepthMoves && depth >= reductionLimit && !inCheck && (getMoveCapture(moveList->moves[count]) == 13) && getMovePromoted(moveList->moves[count]) == 0) {
                score = -negamax(-alpha - 1, -alpha, depth - 2);
            }
            else {
                score = alpha + 1;
            }
            
            // Principal Variation Search (PVS)
            if (score > alpha) {
                score = -negamax(-alpha - 1, -alpha, depth - 1);
            
                if ((score > alpha) && (score < beta)) {
                    score = -negamax(-beta, -alpha, depth - 1);
                }
            }
        }

        ply--;

        takeBack();

        if (Chessboard::stopped) {
            return 0;
        }

        movesSearched++;

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

void Search::searchPosition(int depth) { // 431897 | 274513
    Chessboard::nodes = 0;
    int score = 0;
    Chessboard::stopped = false;
    followPv = false;
    scorePv = false;

    memset(killerMoves, 0, sizeof(killerMoves));
    memset(historyMoves, 0, sizeof(historyMoves));
    memset(pvTable, 0, sizeof(pvTable));
    memset(pvLength, 0, sizeof(pvLength));

    int alpha = -50000;
    int beta = 50000;

    // Iterative Deepening
    for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {
        if (Chessboard::stopped) {
            break;
        }

        followPv = true;

        score = negamax(alpha, beta, currentDepth);

        // Aspiration Window
        if ((score <= alpha) || (score >= beta)) {
            alpha = -50000;
            beta = 50000;
            continue;
        }
        alpha = score - 50;
        beta = score + 50;

        std::cout << "info score cp " << score << " depth " << currentDepth << " nodes " << Chessboard::nodes << " pv ";

        for (int count = 0; count < pvLength[0]; count++) {
            Move::printMove(pvTable[0][count]);
            std::cout << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "bestmove ";
    Move::printMove(pvTable[0][0]);
    std::cout << std::endl;
}