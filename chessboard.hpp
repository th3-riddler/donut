#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "move.hpp"
#include "piece.hpp"


struct BitBoard {
    uint64_t whitePawns = 0;
    uint64_t whiteKnights = 0;
    uint64_t whiteBishops = 0;
    uint64_t whiteRooks = 0;
    uint64_t whiteQueens = 0;
    uint64_t whiteKing = 0;

    uint64_t blackPawns = 0;
    uint64_t blackKnights = 0;
    uint64_t blackBishops = 0;
    uint64_t blackRooks = 0;
    uint64_t blackQueens = 0;
    uint64_t blackKing = 0;

    // Unordered map che contiene puntatori a tutte le bitboard
    std::unordered_map<int, uint64_t*> bitboards = {
        {Piece::WhitePawn, &whitePawns},
        {Piece::WhiteKnight, &whiteKnights},
        {Piece::WhiteBishop, &whiteBishops},
        {Piece::WhiteRook, &whiteRooks},
        {Piece::WhiteQueen, &whiteQueens},
        {Piece::WhiteKing, &whiteKing},
        {Piece::BlackPawn, &blackPawns},
        {Piece::BlackKnight, &blackKnights},
        {Piece::BlackBishop, &blackBishops},
        {Piece::BlackRook, &blackRooks},
        {Piece::BlackQueen, &blackQueens},
        {Piece::BlackKing, &blackKing}
    };

    uint64_t whitePieces() const {
        return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    }

    uint64_t blackPieces() const {
        return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    }

    uint64_t allPieces() const {
        return whitePieces() | blackPieces();
    }

    uint64_t emptySquares() const {
        return ~allPieces();
    }

    uint64_t whiteKingCastle;
    uint64_t whiteQueenCastle;
    uint64_t blackKingCastle;
    uint64_t blackQueenCastle;

    uint64_t enPassantSquare;

    uint64_t halfMoveClock;
    uint64_t fullMoveCounter;

    uint64_t getPiece(int piece) const {
        if (piece == Piece::WhitePawn) {
            return whitePawns;
        } else if (piece == Piece::WhiteKnight) {
            return whiteKnights;
        } else if (piece == Piece::WhiteBishop) {
            return whiteBishops;
        } else if (piece == Piece::WhiteRook) {
            return whiteRooks;
        } else if (piece == Piece::WhiteQueen) {
            return whiteQueens;
        } else if (piece == Piece::WhiteKing) {
            return whiteKing;
        } else if (piece == Piece::BlackPawn) {
            return blackPawns;
        } else if (piece == Piece::BlackKnight) {
            return blackKnights;
        } else if (piece == Piece::BlackBishop) {
            return blackBishops;
        } else if (piece == Piece::BlackRook) {
            return blackRooks;
        } else if (piece == Piece::BlackQueen) {
            return blackQueens;
        } else if (piece == Piece::BlackKing) {
            return blackKing;
        } else {
            return 0;
        }
    }

    void setPiece(int piece, uint64_t value) {
        if (piece == Piece::WhitePawn) {
            whitePawns = value;
        } else if (piece == Piece::WhiteKnight) {
            whiteKnights = value;
        } else if (piece == Piece::WhiteBishop) {
            whiteBishops = value;
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
        std::vector<Move> generatePseudoLegalMoves() const;
        void initializeFromFEN(const std::string& fen);

        BitBoard bitboard;

        int getPieceAt(int file, int rank) const;
        void movePiece(int fromFile, int fromRank, int toFile, int toRank, int piece);

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
        void drawDraggedPiece(int pieceToDraw);

        uint64_t maskPawnAttacks(int square, bool isWhite) const;

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