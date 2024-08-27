#include <iostream>

#include "evaluation.hpp"


const int Evaluation::materialScore[12] = {
    100, // White Pawn
    300, // White Knight
    350, // White Bishop
    500, // White Rook
    900, // White Queen
    10000, // White King
    -100, // Black Pawn
    -300, // Black Knight
    -350, // Black Bishop
    -500, // Black Rook
    -900, // Black Queen
    -10000 // Black King
};

const int Evaluation::pawnScore[64] = {
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

const int Evaluation::knightScore[64] = {
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

const int Evaluation::bishopScore[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  20,   0,  10,  10,   0,  20,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,  10,   0,   0,   0,   0,  10,   0,
    0,  30,   0,   0,   0,   0,  30,   0,
    0,   0, -10,   0,   0, -10,   0,   0
};

const int Evaluation::rookScore[64] = {
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0
};

const int Evaluation::kingScore[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   5,   5,   5,   5,   0,   0,
    0,   5,   5,  10,  10,   5,   5,   0,
    0,   5,  10,  20,  20,  10,   5,   0,
    0,   5,  10,  20,  20,  10,   5,   0,
    0,   0,   5,  10,  10,   5,   0,   0,
    0,   5,   5,  -5,  -5,   0,   5,   0,
    0,   0,   5,   0, -15,   0,  10,   0
};

const int Evaluation::getRank[64] = {
    7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
};

const int Evaluation::doublePawnPenalty = -10;
const int Evaluation::isolatedPawnPenalty = -10;
const int Evaluation::passedPawnBonus[8] = {0, 10, 30, 50, 75, 100, 150, 200};
const int Evaluation::semiOpenFileBonus = 10;
const int Evaluation::openFileBonus = 15;
const int Evaluation::kingShieldBonus = 5;

const int Evaluation::mirrorPieceScore[128] = {
    Chessboard::a8, Chessboard::b8, Chessboard::c8, Chessboard::d8, Chessboard::e8, Chessboard::f8, Chessboard::g8, Chessboard::h8,
    Chessboard::a7, Chessboard::b7, Chessboard::c7, Chessboard::d7, Chessboard::e7, Chessboard::f7, Chessboard::g7, Chessboard::h7,
    Chessboard::a6, Chessboard::b6, Chessboard::c6, Chessboard::d6, Chessboard::e6, Chessboard::f6, Chessboard::g6, Chessboard::h6,
    Chessboard::a5, Chessboard::b5, Chessboard::c5, Chessboard::d5, Chessboard::e5, Chessboard::f5, Chessboard::g5, Chessboard::h5,
    Chessboard::a4, Chessboard::b4, Chessboard::c4, Chessboard::d4, Chessboard::e4, Chessboard::f4, Chessboard::g4, Chessboard::h4,
    Chessboard::a3, Chessboard::b3, Chessboard::c3, Chessboard::d3, Chessboard::e3, Chessboard::f3, Chessboard::g3, Chessboard::h3,
    Chessboard::a2, Chessboard::b2, Chessboard::c2, Chessboard::d2, Chessboard::e2, Chessboard::f2, Chessboard::g2, Chessboard::h2,
    Chessboard::a1, Chessboard::b1, Chessboard::c1, Chessboard::d1, Chessboard::e1, Chessboard::f1, Chessboard::g1, Chessboard::h1
};

const int Evaluation::mvvLva[12][12] = {
    105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

uint64_t Evaluation::fileMasks[64];
uint64_t Evaluation::rankMasks[64];
uint64_t Evaluation::isolatedMasks[64];
uint64_t Evaluation::whitePassedMasks[64];
uint64_t Evaluation::blackPassedMasks[64];


/*  =======================
         Move ordering
    =======================
    
    1. PV move
    2. Captures in MVV/LVA
    3. 1st killer move
    4. 2nd killer move
    5. History moves
    6. Unsorted moves
*/


uint64_t Evaluation::setFileRankMask(int fileNumber, int rankNumber) {
    uint64_t mask = 0ULL;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file;

            if (fileNumber != -1) {
                if (file == fileNumber) {
                    mask |= SET_BIT(mask, square);
                }
            }
            else if (rankNumber != -1) {
                if (rank == rankNumber) {
                    mask |= SET_BIT(mask, square);
                }
            }
        }
    }

    return mask;
}

void Evaluation::initEvalMasks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file;

            fileMasks[square] |= setFileRankMask(file, -1);
            rankMasks[square] |= setFileRankMask(-1, rank);
            isolatedMasks[square] |= setFileRankMask(file - 1, -1) | setFileRankMask(file + 1, -1);
        }
    }

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file;
            whitePassedMasks[square] |= setFileRankMask(file - 1, -1) | setFileRankMask(file, -1) | setFileRankMask(file + 1, -1);
            for (int i = 0; i < (8 - rank); i++) {
                whitePassedMasks[square] &= ~rankMasks[i * 8 + file];
            }
        }
    }

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file;
            blackPassedMasks[square] |= setFileRankMask(file - 1, -1) | setFileRankMask(file, -1) | setFileRankMask(file + 1, -1);
            for (int i = 0; i < rank + 1; i++) {
                blackPassedMasks[square] &= ~rankMasks[(7 - i) * 8 + file];
            }
        }
    }
}

int Evaluation::scoreMove(int move) {
    if (Search::scorePv) {
        if (Search::pvTable[0][Search::ply] == move) {
            Search::scorePv = false;
            return 20000;
        }
    }

    if(getMoveCapture(move) != 13) {
        return mvvLva[getMovePiece(move)][getMoveCapture(move)] + 10000;
    }
    else {
        if (Search::killerMoves[0][Search::ply] == move) {
            return 9000;
        }
        else if (Search::killerMoves[1][Search::ply] == move) {
            return 8000;
        }
        else {
            return Search::historyMoves[getMovePiece(move)][getMoveTarget(move)];
        }
    }
    return 0;
}


int Evaluation::evaluate() {
    int score = 0;

    uint64_t bitboardCopy;
    int piece, square;

    int doublePawns = 0;

    for (int bbPiece = Chessboard::P; bbPiece <= Chessboard::k; bbPiece++) {
        bitboardCopy = Chessboard::bitboard.bitboards[bbPiece];

        while (bitboardCopy) {
            piece = bbPiece;
            square = Chessboard::getLSBIndex(bitboardCopy);

            score += materialScore[piece];

            switch (piece) {
                case Chessboard::P: 
                    score += pawnScore[mirrorPieceScore[square]];
                    doublePawns = Chessboard::countBits(Chessboard::bitboard.bitboards[Chessboard::P] & fileMasks[square]);
                    if (doublePawns > 1) {
                        score += doublePawns * doublePawnPenalty;
                    }

                    if ((Chessboard::bitboard.bitboards[Chessboard::P] & isolatedMasks[square]) == 0) {
                        score += isolatedPawnPenalty;
                    }

                    if ((whitePassedMasks[square] & Chessboard::bitboard.bitboards[Chessboard::p]) == 0) {
                        score += passedPawnBonus[getRank[mirrorPieceScore[square]]];
                    }
                    break;

                case Chessboard::N: score += knightScore[mirrorPieceScore[square]]; break;
                case Chessboard::B: 
                    score += bishopScore[mirrorPieceScore[square]];
                    
                    score += Chessboard::countBits(Move::getBishopAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));

                    break;

                case Chessboard::R: 
                    score += rookScore[mirrorPieceScore[square]];

                    if ((Chessboard::bitboard.bitboards[Chessboard::P] & fileMasks[square]) == 0) {
                        score += semiOpenFileBonus;
                    }

                    if (((Chessboard::bitboard.bitboards[Chessboard::P] | Chessboard::bitboard.bitboards[Chessboard::p]) & fileMasks[square]) == 0) {
                        score += openFileBonus;
                    }

                    break;

                case Chessboard::Q:
                    
                    score += Chessboard::countBits(Move::getQueenAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));

                    break;
                
                case Chessboard::K: 
                    score += kingScore[mirrorPieceScore[square]]; 
                    
                    if ((Chessboard::bitboard.bitboards[Chessboard::P] & fileMasks[square]) == 0) {
                        score -= semiOpenFileBonus;
                    }

                    if (((Chessboard::bitboard.bitboards[Chessboard::P] | Chessboard::bitboard.bitboards[Chessboard::p]) & fileMasks[square]) == 0) {
                        score -= openFileBonus;
                    }

                    score += Chessboard::countBits(Move::kingAttacks[square] & Chessboard::bitboard.occupancies[Chessboard::white]) * kingShieldBonus;

                    break;

                case Chessboard::p: 
                    score -= pawnScore[square];
                    doublePawns = Chessboard::countBits(Chessboard::bitboard.bitboards[Chessboard::p] & fileMasks[square]);
                    if (doublePawns > 1) {
                        score -= doublePawns * doublePawnPenalty;
                    }

                    if ((Chessboard::bitboard.bitboards[Chessboard::p] & isolatedMasks[square]) == 0) {
                        score -= isolatedPawnPenalty;
                    }

                    if ((blackPassedMasks[square] & Chessboard::bitboard.bitboards[Chessboard::P]) == 0) {
                        score -= passedPawnBonus[getRank[square]];
                    }
                    break;
                case Chessboard::n: score -= knightScore[square]; break;
                case Chessboard::b: 
                    score -= bishopScore[square];
                    
                    score -= Chessboard::countBits(Move::getBishopAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));
                    
                    break;
                case Chessboard::r: 
                    score -= rookScore[square];
                    
                    if ((Chessboard::bitboard.bitboards[Chessboard::p] & fileMasks[square]) == 0) {
                        score -= semiOpenFileBonus;
                    }

                    if (((Chessboard::bitboard.bitboards[Chessboard::p] | Chessboard::bitboard.bitboards[Chessboard::P]) & fileMasks[square]) == 0) {
                        score -= openFileBonus;
                    }

                    break;

                case Chessboard::q:
                        
                    score -= Chessboard::countBits(Move::getQueenAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));

                    break;

                case Chessboard::k: 
                    score -= kingScore[square];
                    
                    if ((Chessboard::bitboard.bitboards[Chessboard::p] & fileMasks[square]) == 0) {
                        score += semiOpenFileBonus;
                    }

                    if (((Chessboard::bitboard.bitboards[Chessboard::p] | Chessboard::bitboard.bitboards[Chessboard::P]) & fileMasks[square]) == 0) {
                        score += openFileBonus;
                    }

                    score -= Chessboard::countBits(Move::kingAttacks[square] & Chessboard::bitboard.occupancies[Chessboard::black]) * kingShieldBonus;

                    break;
            }


            CLEAR_BIT(bitboardCopy, square);
        }
    }


    return ((Chessboard::bitboard.sideToMove == Chessboard::white) ? score : -score);
}