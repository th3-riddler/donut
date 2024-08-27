#include <iostream>

#include "evaluation.hpp"


const int Evaluation::materialScore[2][12] = {
    82, 337, 365, 477, 1025, 12000, -82, -337, -365, -477, -1025, -12000,
    94, 281, 297, 512,  936, 12000, -94, -281, -297, -512,  -936, -12000
};

const int Evaluation::openingPhaseScore = 6192;
const int Evaluation::endgamePhaseScore = 518;

const int Evaluation::positionalScore[2][6][64] =

// opening positional piece scores //
{
    //pawn
    0,   0,   0,   0,   0,   0,  0,   0,
    98, 134,  61,  95,  68, 126, 34, -11,
    -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
    0,   0,   0,   0,   0,   0,  0,   0,
    
    // knight
    -167, -89, -34, -49,  61, -97, -15, -107,
    -73, -41,  72,  36,  23,  62,   7,  -17,
    -47,  60,  37,  65,  84, 129,  73,   44,
    -9,  17,  19,  53,  37,  69,  18,   22,
    -13,   4,  16,  13,  28,  19,  21,   -8,
    -23,  -9,  12,  10,  19,  17,  25,  -16,
    -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
    
    // bishop
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -4,   5,  19,  50,  37,  37,   7,  -2,
    -6,  13,  13,  26,  34,  12,  10,   4,
    0,  15,  15,  15,  14,  27,  18,  10,
    4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
    
    // rook
    32,  42,  32,  51, 63,  9,  31,  43,
    27,  32,  58,  62, 80, 67,  26,  44,
    -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
    
    // queen
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -1, -18,  -9,  10, -15, -25, -31, -50,
    
    // king
    -65,  23,  16, -15, -56, -34,   2,  13,
    29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
    1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,


    // Endgame positional piece scores //

    //pawn
    0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
    94, 100,  85,  67,  56,  53,  82,  84,
    32,  24,  13,   5,  -2,   4,  17,  17,
    13,   9,  -3,  -7,  -7,  -8,   3,  -1,
    4,   7,  -6,   1,   0,  -5,  -1,  -8,
    13,   8,   8,  10,  13,   0,   2,  -7,
    0,   0,   0,   0,   0,   0,   0,   0,
    
    // knight
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
    
    // bishop
    -14, -21, -11,  -8, -7,  -9, -17, -24,
    -8,  -4,   7, -12, -3, -13,  -4, -14,
    2,  -8,   0,  -1, -2,   6,   0,   4,
    -3,   9,  12,   9, 14,  10,   3,   2,
    -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
    
    // rook
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
    7,  7,  7,  5,  4,  -3,  -5,  -3,
    4,  3, 13,  1,  2,   1,  -1,   2,
    3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
    
    // queen
    -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
    3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
    
    // king
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
    10,  17,  23,  15,  20,  45,  44,  13,
    -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
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

inline int Evaluation::getGamePhaseScore() {
    /*
        The game phase score of the game is derived from the pieces
        (not counting pawns and kings) that are still on the board.
        The full material starting position game phase score is:
        
        4 * knight material score in the opening +
        4 * bishop material score in the opening +
        4 * rook material score in the opening +
        2 * queen material score in the opening
    */

    int whitePieceScore = 0, blackPieceScore = 0;

    for (int piece = Chessboard::N; piece <= Chessboard::Q; piece++) {
        whitePieceScore += Chessboard::countBits(Chessboard::bitboard.bitboards[piece]) * materialScore[opening][piece];
    }

    for (int piece = Chessboard::n; piece <= Chessboard::q; piece++) {
        blackPieceScore += Chessboard::countBits(Chessboard::bitboard.bitboards[piece]) * -materialScore[opening][piece];
    }

    return whitePieceScore + blackPieceScore;
}

int Evaluation::evaluate() {

    int gamePhaseScore = getGamePhaseScore();

    int gamePhase = -1;

    if (gamePhaseScore > openingPhaseScore) { gamePhase = opening; }
    else if (gamePhaseScore < endgamePhaseScore) { gamePhase = endgame; }
    else { gamePhase = middlegame; }

    int score = 0;

    uint64_t bitboardCopy;
    int piece, square;

    // int doublePawns = 0;

    for (int bbPiece = Chessboard::P; bbPiece <= Chessboard::k; bbPiece++) {
        bitboardCopy = Chessboard::bitboard.bitboards[bbPiece];

        while (bitboardCopy) {
            piece = bbPiece;
            square = Chessboard::getLSBIndex(bitboardCopy);

            /*          
                Now in order to calculate interpolated score
                for a given game phase we use this formula
                (same for material and positional scores):

                (
                    score_opening * game_phase_score + 
                    score_endgame * (opening_phase_score - game_phase_score)
                ) / opening_phase_score

                E.g. the score for pawn on d4 at phase say 5000 would be
                interpolated_score = (12 * 5000 + (-7) * (6192 - 5000)) / 6192 = 8,342377261
            */


            if (gamePhase == middlegame) {
                score += (
                    materialScore[opening][piece] * gamePhaseScore +
                    materialScore[endgame][piece] * (openingPhaseScore - gamePhaseScore)
                ) / openingPhaseScore;
            }
            else {
                score += materialScore[gamePhase][piece];
            }
            
            switch (piece) {
                case Chessboard::P:

                    if (gamePhase == middlegame) {
                        score += (
                        positionalScore[opening][PAWN][mirrorPieceScore[square]] * gamePhaseScore +
                        positionalScore[endgame][PAWN][mirrorPieceScore[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score += positionalScore[gamePhase][PAWN][mirrorPieceScore[square]];
                    }
                    
                    /*doublePawns = Chessboard::countBits(Chessboard::bitboard.bitboards[Chessboard::P] & fileMasks[square]);
                    if (doublePawns > 1) {
                        score += doublePawns * doublePawnPenalty;
                    }

                    if ((Chessboard::bitboard.bitboards[Chessboard::P] & isolatedMasks[square]) == 0) {
                        score += isolatedPawnPenalty;
                    }

                    if ((whitePassedMasks[square] & Chessboard::bitboard.bitboards[Chessboard::p]) == 0) {
                        score += passedPawnBonus[getRank[mirrorPieceScore[square]]];
                    }
                    */
                    break;
                case Chessboard::N: 

                    if (gamePhase == middlegame) {
                        score += (
                        positionalScore[opening][KNIGHT][mirrorPieceScore[square]] * gamePhaseScore +
                        positionalScore[endgame][KNIGHT][mirrorPieceScore[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score += positionalScore[gamePhase][KNIGHT][mirrorPieceScore[square]];
                    }

                    break;
                case Chessboard::B: 
                    
                    if (gamePhase == middlegame) {
                        score += (
                        positionalScore[opening][BISHOP][mirrorPieceScore[square]] * gamePhaseScore +
                        positionalScore[endgame][BISHOP][mirrorPieceScore[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score += positionalScore[gamePhase][BISHOP][mirrorPieceScore[square]];
                    }

                    /*score += Chessboard::countBits(Move::getBishopAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));
                    */
                    break;

                case Chessboard::R:

                    if (gamePhase == middlegame) {
                        score += (
                        positionalScore[opening][ROOK][mirrorPieceScore[square]] * gamePhaseScore +
                        positionalScore[endgame][ROOK][mirrorPieceScore[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score += positionalScore[gamePhase][ROOK][mirrorPieceScore[square]];
                    }

                    /*if ((Chessboard::bitboard.bitboards[Chessboard::P] & fileMasks[square]) == 0) {
                        score += semiOpenFileBonus;
                    }

                    if (((Chessboard::bitboard.bitboards[Chessboard::P] | Chessboard::bitboard.bitboards[Chessboard::p]) & fileMasks[square]) == 0) {
                        score += openFileBonus;
                    }
                    */
                    break;

                case Chessboard::Q:
                    
                    if (gamePhase == middlegame) {
                        score += (
                        positionalScore[opening][QUEEN][mirrorPieceScore[square]] * gamePhaseScore +
                        positionalScore[endgame][QUEEN][mirrorPieceScore[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score += positionalScore[gamePhase][QUEEN][mirrorPieceScore[square]];
                    }

                    // score += Chessboard::countBits(Move::getQueenAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));

                    break;
                
                case Chessboard::K: 
                    
                    if (gamePhase == middlegame) {
                        score += (
                        positionalScore[opening][KING][mirrorPieceScore[square]] * gamePhaseScore +
                        positionalScore[endgame][KING][mirrorPieceScore[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score += positionalScore[gamePhase][KING][mirrorPieceScore[square]];
                    }

                    /*if ((Chessboard::bitboard.bitboards[Chessboard::P] & fileMasks[square]) == 0)
                        // add semi open file penalty
                        score -= semiOpenFileBonus;
                    
                    // semi open file
                    if (((Chessboard::bitboard.bitboards[Chessboard::P] | Chessboard::bitboard.bitboards[Chessboard::p]) & fileMasks[square]) == 0)
                        // add semi open file penalty
                        score -= openFileBonus;
                    
                    // king safety bonus
                    score += Chessboard::countBits(Move::kingAttacks[square] & Chessboard::bitboard.occupancies[Chessboard::white]) * kingShieldBonus;
                    */

                    break;

                case Chessboard::p:

                    if (gamePhase == middlegame) {
                        score -= (
                        positionalScore[opening][PAWN][square] * gamePhaseScore +
                        positionalScore[endgame][PAWN][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score -= positionalScore[gamePhase][PAWN][square];
                    }

                    /*doublePawns = Chessboard::countBits(Chessboard::bitboard.bitboards[Chessboard::p] & fileMasks[square]);
                    if (doublePawns > 1) {
                        score -= doublePawns * doublePawnPenalty;
                    }

                    if ((Chessboard::bitboard.bitboards[Chessboard::p] & isolatedMasks[square]) == 0) {
                        score -= isolatedPawnPenalty;
                    }

                    if ((blackPassedMasks[square] & Chessboard::bitboard.bitboards[Chessboard::P]) == 0) {
                        score -= passedPawnBonus[getRank[square]];
                    }*/
                    break;
                case Chessboard::n:

                    if (gamePhase == middlegame) {
                        score -= (
                        positionalScore[opening][KNIGHT][square] * gamePhaseScore +
                        positionalScore[endgame][KNIGHT][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score -= positionalScore[gamePhase][KNIGHT][square];
                    }

                    break;
                case Chessboard::b:

                    if (gamePhase == middlegame) {
                        score -= (
                        positionalScore[opening][BISHOP][square] * gamePhaseScore +
                        positionalScore[endgame][BISHOP][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score -= positionalScore[gamePhase][BISHOP][square];
                    }
                    
                    // score -= Chessboard::countBits(Move::getBishopAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));
                    
                    break;
                case Chessboard::r: 
                    
                    if (gamePhase == middlegame) {
                        score -= (
                        positionalScore[opening][ROOK][square] * gamePhaseScore +
                        positionalScore[endgame][ROOK][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score -= positionalScore[gamePhase][ROOK][square];
                    }

                    /*if ((Chessboard::bitboard.bitboards[Chessboard::p] & fileMasks[square]) == 0) {
                        score -= semiOpenFileBonus;
                    }

                    if (((Chessboard::bitboard.bitboards[Chessboard::p] | Chessboard::bitboard.bitboards[Chessboard::P]) & fileMasks[square]) == 0) {
                        score -= openFileBonus;
                    }*/

                    break;

                case Chessboard::q:
                    
                    if (gamePhase == middlegame) {
                        score -= (
                        positionalScore[opening][QUEEN][square] * gamePhaseScore +
                        positionalScore[endgame][QUEEN][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score -= positionalScore[gamePhase][QUEEN][square];
                    }

                    // score -= Chessboard::countBits(Move::getQueenAttacks(square, Chessboard::bitboard.occupancies[Chessboard::both]));

                    break;

                case Chessboard::k: 
                    
                    if (gamePhase == middlegame) {
                        score -= (
                        positionalScore[opening][KING][square] * gamePhaseScore +
                        positionalScore[endgame][KING][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;
                    }
                    else {
                        score -= positionalScore[gamePhase][KING][square];
                    }

                    /*if ((Chessboard::bitboard.bitboards[Chessboard::p] & fileMasks[square]) == 0) {
                        score += semiOpenFileBonus;
                    }

                    if (((Chessboard::bitboard.bitboards[Chessboard::p] | Chessboard::bitboard.bitboards[Chessboard::P]) & fileMasks[square]) == 0) {
                        score += openFileBonus;
                    }

                    score -= Chessboard::countBits(Move::kingAttacks[square] & Chessboard::bitboard.occupancies[Chessboard::black]) * kingShieldBonus;
                    */
                    break;
            }


            CLEAR_BIT(bitboardCopy, square);
        }
    }


    return ((Chessboard::bitboard.sideToMove == Chessboard::white) ? score : -score);
}