#include <iostream>

#include "evaluation.hpp"

int Evaluation::nnuePieces[12] = { 6, 5, 4, 3, 2, 1, 12, 11, 10, 9, 8, 7 };

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

    // int scoree = (evaluate_nnue(Chessboard::bitboard.sideToMove, pieces, squares) * (100 - Search::fifty) / 100);
    // std::cout << "Score Evalaute(): " << scoree << std::endl;

    return (evaluate_nnue(Chessboard::bitboard.sideToMove, pieces, squares) * (100 - Search::fifty) / 100);
}