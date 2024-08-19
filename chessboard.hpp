#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

#include "move.hpp"
#include "piece.hpp"
#include "bitboard.hpp"

class Move;
struct moves;

class Chessboard {
    public:
        explicit Chessboard(SDL_Renderer* renderer);
        ~Chessboard();

        void drawGameState();
        void handleEvent(const SDL_Event& e);
        static void parseFEN(const char *fen);

        static BitBoard bitboard;

        int getPieceAt(int file, int rank) const;
        void movePiece(int fromFile, int fromRank, int toFile, int toRank, int piece);

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
        SDL_Renderer* renderer;
        const int squareSize = 100;
        const int boardSize = 8;

        enum { wk = 1, wq = 2, bk = 4, bq = 8 };
        enum { allMoves, capturesOnly };

        static char asciiPieces[13];
        static const char *unicodePieces[13]; // ♔ 	♕ 	♖ 	♗ 	♘ 	♙ 	♚ 	♛ 	♜ 	♝ 	♞ 	♟
        static int charPieces[128]; // Inizializza tutto a 0
        
        static inline void initCharPieces();
        

        std::unordered_map<char, SDL_Texture*> pieceTextures; // Maps piece characters to textures

        void loadPieceTextures();

        void draw();
        void drawPieces();
        void drawDraggedPiece(int pieceToDraw);
        void generateSlidingMoves(int startSquare, int piece);
        void generateLeapingMoves(int startSquare, int piece);

        static void printBoard();
        static void printMove(int move);
        static void printMoveList(moves *moveList);

        void colorSquare();
        bool isInValidMoveList(int file, int rank, int piece);
        
        void initAll();

        static inline bool isSquareAttacked(int square, int side);
        static void printAttackedSquares(int side);

        static inline void generateMoves(moves *moveList);
        static inline int makeMove(int move, int moveFlag);

        // Drag-and-drop related members
        bool dragging = false;
        int draggedPiece = 0;
        int draggedPieceStartX = -1;
        int draggedPieceStartY = -1;
        int draggedPieceX = -1;
        int draggedPieceY = -1;
        int mouseX = -1;
        int mouseY = -1;
};


#endif // CHESSBOARD_HPP