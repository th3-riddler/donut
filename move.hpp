#ifndef MOVE_HPP
#define MOVE_HPP

#include "piece.hpp" // Include the header file where PieceType is defined

struct Move {
    int fromFile;
    int fromRank;
    int toFile;
    int toRank;
    int piece;

    Move(int fromFile, int fromRank, int toFile, int toRank, int piece)
        : fromFile(fromFile), fromRank(fromRank), toFile(toFile), toRank(toRank), piece(piece) {}
};

#endif // MOVE_HPP