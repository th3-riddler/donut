// bitwise_operations.h
#ifndef BITWISE_OPERATIONS_H
#define BITWISE_OPERATIONS_H

// Macro to set a bit at a given position
#define SET_BIT(board, pos) ((board) |= (1ULL << (pos)))

// Macro to clear a bit at a given position
#define CLEAR_BIT(board, pos) ((board) &= ~(1ULL << (pos)))

// Macro to toggle a bit at a given position
#define TOGGLE_BIT(board, pos) ((board) ^= (1ULL << (pos)))

// Macro to check if a bit at a given position is set
#define GET_BIT(board, pos) (((board) & (1ULL << (pos))) != 0) 

#endif // BITWISE_OPERATIONS_H