// bitwise_operations.h
#ifndef BITWISE_OPERATIONS_H
#define BITWISE_OPERATIONS_H

// Macro to set a bit at a given position
#define SET_BIT(board, square) ((board) |= (1ULL << (square)))

// Macro to clear a bit at a given position
#define CLEAR_BIT(board, square) ((board) &= ~(1ULL << (square)))

// Macro to toggle a bit at a given position
#define TOGGLE_BIT(board, square) ((board) ^= (1ULL << (square)))

// Macro to check if a bit at a given position is set
#define GET_BIT(board, square) (((board) & (1ULL << (square))) != 0)

#define emptyBoard (const char*)"8/8/8/8/8/8/8/8 b - - "

#define startPosition (const char*)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define trickyPosition (const char*)"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define killerPosition (const char*)"rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

#define cmkPosition (const char*)"r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9"

#define encodeMove(source, target, piece, promoted, capture, doublePush, enPassant, castling) \
    ((source) | ((target) << 6) | ((piece) << 12) | ((promoted) << 16) | ((capture) << 20) | ((doublePush) << 21) | ((enPassant) << 22) | ((castling) << 23))

#define getMoveSource(move) ((move) & 0x3F)

#define getMoveTarget(move) ((move & 0xFC0) >> 6)

#define getMovePiece(move) ((move & 0xF000) >> 12)

#define getMovePromoted(move) ((move & 0xF0000) >> 16)

#define getMoveCapture(move) (move & 0x100000)

#define getMoveDoublePush(move) (move & 0x200000)

#define getMoveEnPassant(move) (move & 0x400000)

#define getMoveCastling(move) (move & 0x800000)

#define copyBoard()                                         \
    uint64_t bitboardsCopy[12], occupanciesCopy[3];         \
    int sideCopy, enPassantSquareCopy, castlingRightsCopy;  \
    memcpy(bitboardsCopy, bitboard.bitboards, 96);          \
    memcpy(occupanciesCopy, bitboard.occupancies, 24);      \
    sideCopy = bitboard.sideToMove;                         \
    enPassantSquareCopy = bitboard.enPassantSquare;         \
    castlingRightsCopy = bitboard.castlingRights;           \

#define takeBack()                                          \
    memcpy(bitboard.bitboards, bitboardsCopy, 96);          \
    memcpy(bitboard.occupancies, occupanciesCopy, 24);      \
    bitboard.sideToMove = sideCopy;                         \
    bitboard.enPassantSquare = enPassantSquareCopy;         \
    bitboard.castlingRights = castlingRightsCopy;           \

#endif // BITWISE_OPERATIONS_H