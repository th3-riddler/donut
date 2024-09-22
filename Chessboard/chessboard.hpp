#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#include "../Move/move.hpp"
#include "../Macros/bitboard.hpp"
#include "../Macros/macros.hpp"
#include "../Search/search.hpp"
#include "../Evaluation/evaluation.hpp"
#include "../nnueEval/nnueEval.hpp"
#include "../Polyglot/polyglot.hpp"

class Move;
class Evaluation;
struct moves;
struct tt;

class Chessboard {
    public:
        static void parseFEN(char *fen);

        static BitBoard bitboard;
        static uint64_t nodes;
        static bool stopped;

        // Zobrist Keys
        static uint64_t pieceKeys[12][64];
        static uint64_t enPassantKeys[64];
        static uint64_t castleKeys[16];
        static uint64_t sideKey;
        static uint64_t hashKey;

        static uint64_t polyKeyFromBoard();
        static const int polyPieces[12];

        static bool canPawnEnPassant();

        static char asciiPieces[13];
        static const char *unicodePieces[13]; // ♔ 	♕ 	♖ 	♗ 	♘ 	♙ 	♚ 	♛ 	♜ 	♝ 	♞ 	♟

        static void init();
        static void communicate();
        static int getTimeMs();

        static void printBitboards(uint64_t bitboard);
        static void resetBoard();

        static void generateMoves(moves *moveList);
        static int makeMove(int move, int moveFlag);

        static bool isSquareAttacked(int square, int side);

        enum { white, black, both };
        enum { rook, bishop };
        enum { P, N, B, R, Q, K, p, n, b, r, q, k };
        enum { allMoves, capturesOnly };
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
        static int startTime;
    private:
        static bool quit;
        static int movesToGo;
        static int moveTime;
        static int time;
        static int inc;
        
        static int stopTime;
        static int timeSet;

        enum { wk = 1, wq = 2, bk = 4, bq = 8 };

        static int charPieces[128]; // Inizializza tutto a 0
        
        static inline void initCharPieces();

        static void printBoard();
        
        static void printAttackedSquares(int side);

        static inline void perftDriver(int depth);
        static void perftTest(int depth);
        static int parseMove(char *moveString);
        static void parsePosition(char *command);
        static void parseGo(char *command);
        static void uciLoop();
        static int inputWaiting();
        static void readInput();
        static void resetTimeControl();

        static void initRandomKeys();
        static uint64_t generateHashKey();
};


#endif // CHESSBOARD_HPP