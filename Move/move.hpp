#ifndef MOVE_HPP
#define MOVE_HPP

#include "../Chessboard/chessboard.hpp"
#include "../Macros/macros.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

class Chessboard;

struct moves {
    int moves[256];
    int count;
};

class Move {
    public:
        static uint64_t getBishopAttacks(int square, uint64_t occupancy);
        static uint64_t getRookAttacks(int square, uint64_t occupancy);
        static uint64_t getQueenAttacks(int square, uint64_t occupancy);

        static void printMoveList(moves *moveList);
        static void printMoveScores(moves *moveList);
        static void printMove(int move);
        static void printLegalMoves(moves *moveList);
        static void sortMoves(moves *moveList);

        static uint64_t getRandomU64Number();
        static void initMagicNumbers();

        static void init();

        static uint64_t bishopMasks[64];
        static uint64_t rookMasks[64];

        static uint64_t bishopAttacks[64][512];
        static uint64_t rookAttacks[64][4096];
        static uint64_t pawnAttacks[2][64];
        static uint64_t knightAttacks[64];
        static uint64_t kingAttacks[64];

        static char promotedPieces[11];
        static const int castlingRightsMask[64];

        static inline void addMove(moves *moveList, int move) {
            moveList->moves[moveList->count++] = move;
        }

    private:

        static uint64_t maskPawnAttacks(int square, int color);
        static uint64_t maskKnightAttacks(int square);
        static uint64_t maskKingAttacks(int square);
        static uint64_t maskBishopAttacks(int square);
        static uint64_t maskRookAttacks(int square);

        static uint64_t bishopAttacksOnTheFly(int square, uint64_t block);
        static uint64_t rookAttacksOnTheFly(int square, uint64_t block);

        static uint64_t setOccupancy(int index, int bitsInMask, uint64_t attackMask);
        static unsigned int getRandomU32Number();
        static uint64_t generateMagicNumber();
        static uint64_t findMagicNumber(int square, int relevantBits, int bishop);

        static void initLeapersAttacks();
        static void initSlidingAttacks(int bishop);
        static void initPromotedPieces();
};


#endif // MOVE_HPP