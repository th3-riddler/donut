#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "move.hpp"
#include "pieceType.hpp"


struct BitBoard {
    std::unordered_map<PieceType, uint64_t> bitboards; // Maps piece types to bitboards

    uint64_t allPieces;
    // uint64_t emptySquares;
    uint64_t enPassantSquare;
    uint64_t castlingRights;
    
    bool whiteKingCastle;
    bool whiteQueenCastle;
    bool blackKingCastle;
    bool blackQueenCastle;
    int halfMoveClock;
    int fullMoveCounter;

    // Constructor to initialize the bitboards map
    BitBoard() {
        for (int i = WHITE_PAWN; i < PIECE_TYPE_COUNT; ++i) {
            bitboards[static_cast<PieceType>(i)] = 0;
        }
    }
};


class Chessboard {
    public:
        explicit Chessboard(SDL_Renderer* renderer);
        ~Chessboard();

        void drawGameState();
        void handleEvent(const SDL_Event& e);

        std::vector<Move> generateMoves() const;
        void initializeFromFEN(const std::string& fen);

        BitBoard bitboard;

        PieceType getPieceAt(int file, int rank) const;
        void movePiece(int fromFile, int fromRank, int toFile, int toRank, PieceType piece);

        void printBitboards() const;

        bool whiteToMove = true;
    private:
        SDL_Renderer* renderer;
        const int squareSize = 100;
        const int boardSize = 8;

        std::unordered_map<char, SDL_Texture*> pieceTextures; // Maps piece characters to textures

        void parseFEN(const std::string& fen);
        void loadPieceTextures();

        void draw();
        void drawPieces();
        void drawDraggedPiece(PieceType pieceToDraw);

        uint64_t maskPawnAttacks(int square, bool isWhite) const;

        // Drag-and-drop related members
        bool dragging = false;
        PieceType draggedPiece = PIECE_TYPE_COUNT;
        int draggedPieceStartX = -1;
        int draggedPieceStartY = -1;
        int draggedPieceX = -1;
        int draggedPieceY = -1;
        int mouseX = -1;
        int mouseY = -1;
};

#endif // CHESSBOARD_HPP