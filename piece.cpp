#include <map>
#include <cctype>
#include <cstdint>

#include "piece.hpp"


// Piece Types
const int Piece::None = 0; // 0b0000
const int Piece::Pawn = 1; // 0b0001
const int Piece::Knight = 2; // 0b0010
const int Piece::Bishop = 3; // 0b0011
const int Piece::Rook = 4; // 0b0100
const int Piece::Queen = 5; // 0b0101
const int Piece::King = 6; // 0b0110

// Piece Colors
const int Piece::White = 0; // 0b0000
const int Piece::Black = 8; // 0b1000

// Pieces
const int Piece::WhitePawn = Pawn | White; // 1 --> 0001
const int Piece::WhiteKnight = Knight | White; // 2 --> 0010
const int Piece::WhiteBishop = Bishop | White; // 3 --> 0011
const int Piece::WhiteRook = Rook | White; // 4 --> 0100
const int Piece::WhiteQueen = Queen | White; // 5 --> 0101
const int Piece::WhiteKing = King | White; // 6 --> 0110

const int Piece::BlackPawn = Pawn | Black; // 9 --> 1001
const int Piece::BlackKnight = Knight | Black; // 10 --> 1010
const int Piece::BlackBishop = Bishop | Black; // 11 --> 1011
const int Piece::BlackRook = Rook | Black; // 12 --> 1100
const int Piece::BlackQueen = Queen | Black; // 13 --> 1101
const int Piece::BlackKing = King | Black; // 14 --> 1110

const int Piece::pieceMask = 0b0111;
const int Piece::colorMask = 0b1000;

bool Piece::isColor (int piece, int color) { return (piece & colorMask) == color && piece != 0; }
bool Piece::isWhite (int piece) { return isColor(piece, White);}

int Piece::getType (int piece) { return piece & pieceMask; }

char Piece::getSymbol (int piece) {
    int pieceType = Piece::getType(piece);
    char symbol;
    switch (pieceType) {
        case 1: // Pawn
            symbol = 'P';
            break;
        case 2: // Knight
            symbol = 'N';
            break;
        case 3: // Bishop
            symbol = 'B';
            break;
        case 4: // Rook
            symbol = 'R';
            break;
        case 5: // Queen
            symbol = 'Q';
            break;
        case 6: // King
            symbol = 'K';
            break;
        default: // Empty square
            symbol = ' ';
            break;
    }
    return Piece::isWhite(piece) ? symbol : tolower(symbol);
}