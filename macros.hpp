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

#define emptyBoard (const char*)"8/8/8/8/8/8/8/8 w - - "

#define startPosition (const char*)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define trickyPosition (const char*)"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define killerPosition (const char*)"rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

#define cmkPosition (const char*)"r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9"

#endif // BITWISE_OPERATIONS_H