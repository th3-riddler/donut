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


inline void Search::enablePvScore(moves *moveList) {
    followPv = false;

    for (int count = 0; count < moveList->count; count++) {
        if (pvTable[0][ply] == moveList->moves[count]) {
            scorePv = true;
            followPv = true;
        }
    }
}

void Search::clearTranspositionTable() {
    for (int index = 0; index < hashSize; index++) {
        transpositionTable[index].hashKey = 0;
        transpositionTable[index].depth = 0;
        transpositionTable[index].flags = 0;
        transpositionTable[index].score = 0;
    }
}

int Search::readHashEntry(int alpha, int beta, int depth) {
    tt *ttEntry = &transpositionTable[Chessboard::hashKey % hashSize];

    if (ttEntry->hashKey == Chessboard::hashKey) {
        if (ttEntry->depth >= depth) {
            if (ttEntry->flags == hashFlagExact) {
                return ttEntry->score;
            }
            if ((ttEntry->flags == hashFlagAlpha) && (ttEntry->score <= alpha)) {
                return alpha;
            }
            if ((ttEntry->flags == hashFlagBeta) && (ttEntry->score >= beta)) {
                return beta;
            }
        }
    }

    return noHashEntry;
}

void Search::writeHashEntry(int score, int depth, int flag) {
    tt *ttEntry = &transpositionTable[Chessboard::hashKey % hashSize];

    ttEntry->hashKey = Chessboard::hashKey;
    ttEntry->score = score;
    ttEntry->flags = flag;
    ttEntry->depth = depth;
}

int Search::quiescenceSearch(int alpha, int beta) {

    if ((Chessboard::nodes & 2047) == 0) {
        Chessboard::communicate();
    }

    Chessboard::nodes++;
    
    if (ply > maxPly - 1) {
        return Evaluation::evaluate();
    }

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

        if (Chessboard::stopped) {
            return 0;
        }

        

        if (score > alpha) {
            alpha = score;

            // Fail hard beta-cutoff
            if (score >= beta) {
                return beta;
            }
        }
    }

    return alpha;
}

int Search::negamax(int alpha, int beta, int depth) {

    int score = 0;

    int hashFlag = hashFlagAlpha;

    if (ply && ((score = readHashEntry(alpha, beta, depth)) != noHashEntry)) {
        return score;
    }

    if ((Chessboard::nodes & 2047) == 0) {
        Chessboard::communicate();
    }

    pvLength[ply] = ply;

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

    // Null Move Pruning
    if (depth >= 3 && !inCheck && ply) {
        copyBoard();

        ply++;

        if (Chessboard::bitboard.enPassantSquare != Chessboard::noSquare) {
            Chessboard::hashKey ^= Chessboard::enPassantKeys[Chessboard::bitboard.enPassantSquare];
        }
        Chessboard::bitboard.enPassantSquare = Chessboard::noSquare;

        Chessboard::bitboard.sideToMove ^= 1;
        Chessboard::hashKey ^= Chessboard::sideKey;

        // Search Moves with a reduced depth (depth - 1 - R) --> R is the reduction amount
        score = -negamax(-beta, -beta + 1, depth - 1 - 2);

        ply--;

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

        if (score > alpha) {
            hashFlag = hashFlagExact;
            if (getMoveCapture(moveList->moves[count]) == 13) {
                historyMoves[getMovePiece(moveList->moves[count])][getMoveTarget(moveList->moves[count])] += depth;
            }

            alpha = score;

            pvTable[ply][ply] = moveList->moves[count];
            for (int nextPly = ply + 1; nextPly < pvLength[ply + 1]; nextPly++) {
                pvTable[ply][nextPly] = pvTable[ply + 1][nextPly]; // Copy the PV line from the next ply to the current ply
            }
            pvLength[ply] = pvLength[ply + 1];

            // Fail hard beta-cutoff
            if (score >= beta) {
                writeHashEntry(beta, depth, hashFlagBeta);
                if (getMoveCapture(moveList->moves[count]) == 13) {
                    killerMoves[1][ply] = killerMoves[0][ply];
                    killerMoves[0][ply] = moveList->moves[count];
                }
                return beta;
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

    writeHashEntry(alpha, depth, hashFlag);

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