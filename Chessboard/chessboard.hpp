#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

#include "../Move/move.hpp"
#include "../Macros/bitboard.hpp"

class Move;
struct moves;

class Chessboard {
    public:
        static void parseFEN(const char *fen);

        static BitBoard bitboard;

        static void init();
        static void getPawnMoves();
        static void printBitboards(uint64_t bitboard);

        enum { white, black, both };
        enum { rook, bishop };
        enum { P, N, B, R, Q, K, p, n, b, r, q, k };
        enum {
            a1, b1, c1, d1, e1, f1, g1, h1,
            a2, b2, c2, d2, e2, f2, g2, h2,
            a3, b3, c3, d3, e3, f3, g3, h3,
            a4, b4, c4, d4, e4, f4, g4, h4,
            a5, b5, c5, d5, e5, f5, g5, h5,
            a6, b6, c6, d6, e6, f6, g6, h6,
            a7, b7, c7, d7, e7, f7, g7, h7,
            a8, b8, c8, d8, e8, f8, g8, h8, noSquare
        };

        static const char* squareToCoordinates[64];

        // Count bits within a bitboard
        static inline int countBits(uint64_t bitboard) {
            int count = 0;
            while (bitboard) {
                count++;
                bitboard &= bitboard - 1;
            }
            return count;
        }

        // Get the index of the least significant bit
        static inline int getLSBIndex(uint64_t bitboard) {
            if (bitboard)
                return countBits((bitboard & -bitboard) - 1);
            else
                return -1;
        }


    private:
        const int squareSize = 100;
        const int boardSize = 8;

        enum { wk = 1, wq = 2, bk = 4, bq = 8 };
        enum { allMoves, capturesOnly };

        static char asciiPieces[13];
        static const char *unicodePieces[13]; // ♔ 	♕ 	♖ 	♗ 	♘ 	♙ 	♚ 	♛ 	♜ 	♝ 	♞ 	♟
        static int charPieces[128]; // Inizializza tutto a 0
        static long nodes;
        
        static inline void initCharPieces();

        static void printBoard();
        static void printMove(int move);
        static void printMoveList(moves *moveList);
        
        static inline bool isSquareAttacked(int square, int side);
        static void printAttackedSquares(int side);

        static inline void generateMoves(moves *moveList);
        static inline bool makeMove(int move, int moveFlag);
        static inline int getTimeMs();
        static inline void perftDriver(int depth);
        static void perftTest(int depth);
        static int parseMove(char *moveString);
};


#endif // CHESSBOARD_HPP