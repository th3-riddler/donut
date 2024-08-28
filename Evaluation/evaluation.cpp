#include <iostream>

#include "evaluation.hpp"

int Evaluation::nnuePieces[12] = { 6, 5, 4, 3, 2, 1, 12, 11, 10, 9, 8, 7 };

int Evaluation::nnueSquares[64] = {
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

void Evaluation::nnueInput(int *pieces, int *squares) {
    uint64_t bitboardCopy;
    int piece, square;
    int index = 2;

    for (int bbPiece = Chessboard::P; bbPiece <= Chessboard::k; bbPiece++) {
        bitboardCopy = Chessboard::bitboard.bitboards[bbPiece];

        while (bitboardCopy) {
            piece = bbPiece;
            square = Chessboard::getLSBIndex(bitboardCopy);

            if (piece == Chessboard::K) {
                pieces[0] = nnuePieces[piece];
                squares[0] = square;
            }
            else if (piece == Chessboard::k) {
                pieces[1] = nnuePieces[piece];
                squares[1] = square;
            }
            else {
                pieces[index] = nnuePieces[piece];
                squares[index] = square;
                index++;
            }

            CLEAR_BIT(bitboardCopy, square);
        }
    }
    pieces[index] = 0;
    squares[index] = 0;
}

int Evaluation::evaluate() {

    uint64_t bitboardCopy;
    
    int piece, square;

    int pieces[33];
    int squares[33];
    int index = 2;

    for (int bbPiece = Chessboard::P; bbPiece <= Chessboard::k; bbPiece++) {
        bitboardCopy = Chessboard::bitboard.bitboards[bbPiece];

        while (bitboardCopy) {
            piece = bbPiece;
            square = Chessboard::getLSBIndex(bitboardCopy);
           
            if (piece == Chessboard::K) {
                pieces[0] = nnuePieces[piece];
                squares[0] = square;
            }
            else if (piece == Chessboard::k) {
                pieces[1] = nnuePieces[piece];
                squares[1] = square;
            }
            else {
                pieces[index] = nnuePieces[piece];
                squares[index] = square;
                index++;
            }
            CLEAR_BIT(bitboardCopy, square);
        }
    }

    pieces[index] = 0;
    squares[index] = 0;

    return evaluate_nnue(Chessboard::bitboard.sideToMove, pieces, squares) * (100 - Search::fifty) / 100;
}