#ifndef PIECE_HPP
#define PIECE_HPP

#include <map>
#include <cctype>
#include <cstdint>


class Piece {
    public:
        // Piece Types
        static const int None;
        static const int Pawn;
        static const int Knight;
        static const int Bishop;
        static const int Rook;
        static const int Queen;
        static const int King;

        // Piece Colors
        static const int White;
        static const int Black;

        // Pieces
        static const int WhitePawn;
        static const int WhiteKnight;
        static const int WhiteBishop;
        static const int WhiteRook;
        static const int WhiteQueen;
        static const int WhiteKing;

        static const int BlackPawn;
        static const int BlackKnight;
        static const int BlackBishop;
        static const int BlackRook;
        static const int BlackQueen;
        static const int BlackKing;

        static const int pieceMask;
        static const int colorMask;

        static bool isColor (int piece, int color);
        static bool isWhite (int piece);

        static int getType (int piece);

        static char getSymbol (int piece);

        static bool isSlidingPiece (int piece);
};

#endif // PIECE_HPP