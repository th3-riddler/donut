// piecetype.hpp
#ifndef PIECETYPE_HPP
#define PIECETYPE_HPP

#include <map>

enum PieceType {
    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING,
    PIECE_TYPE_COUNT // This should be the last element
};

const std::map<PieceType, char> pieceTypeToChar = {
    {WHITE_PAWN, 'P'},
    {WHITE_KNIGHT, 'N'},
    {WHITE_BISHOP, 'B'},
    {WHITE_ROOK, 'R'},
    {WHITE_QUEEN, 'Q'},
    {WHITE_KING, 'K'},
    {BLACK_PAWN, 'p'},
    {BLACK_KNIGHT, 'n'},
    {BLACK_BISHOP, 'b'},
    {BLACK_ROOK, 'r'},
    {BLACK_QUEEN, 'q'},
    {BLACK_KING, 'k'}
};

#endif // PIECETYPE_HPP