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


        std::vector<Move> moves;

        std::vector<Move> generateMoves();
        std::vector<Move> generatePseudoLegalMoves();
        void initializeFromFEN(const std::string& fen);

        BitBoard bitboard;

        int getPieceAt(int file, int rank) const;
        void movePiece(int fromFile, int fromRank, int toFile, int toRank, int piece);

        void printBitboards(uint64_t bitboard) const;

        int sideToMove = 0; // 0 = white, 8 = black
    private:
        SDL_Renderer* renderer;
        const int squareSize = 100;
        const int boardSize = 8;

        const char *squareToCoordinates[64] = {
            "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
            "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
            "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
            "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
            "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
            "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
            "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
            "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
        };

        std::unordered_map<char, SDL_Texture*> pieceTextures; // Maps piece characters to textures

        void parseFEN(const std::string& fen);
        void loadPieceTextures();

        void draw();
        void drawPieces();
        void drawDraggedPiece(int pieceToDraw);
        void generateSlidingMoves(int startSquare, int piece);
        void generateLeapingMoves(int startSquare, int piece);

        void colorSquare();
        bool isInValidMoveList(int file, int rank, int piece);
        
        void getPawnMoves(int square, int piece);

        uint64_t maskPawnAttacks(int square, bool isWhite) const;
        uint64_t maskKnightAttacks(int square) const;
        uint64_t maskKingAttacks(int square) const;
        uint64_t maskBishopAttacks(int square) const;
        uint64_t maskRookAttacks(int square) const;

        uint64_t bishopAttackOnTheFly(int square, uint64_t block) const;
        uint64_t rookAttacksOnTheFly(int square, uint64_t block) const;

        uint64_t setOccupancy(int index, int bitsInMask, uint64_t attackMask) const;
        unsigned int getRandomU32Number() const;
        uint64_t getRandomU64Number() const;
        uint64_t generateMagicNumber() const;


        // Count bits within a bitboard
        static inline int countBits(uint64_t bitboard) {
            int count = 0;
            while (bitboard) {
                count++;
                bitboard &= bitboard - 1;
            }
            return count;
        }

        static inline int getLSBIndex(uint64_t bitboard) {
            if (bitboard)
                return countBits((bitboard & -bitboard) - 1);
            else
                return -1;
        }

        // Drag-and-drop related members
        bool dragging = false;
        int draggedPiece = 0;
        int draggedPieceStartX = -1;
        int draggedPieceStartY = -1;
        int draggedPieceX = -1;
        int draggedPieceY = -1;
        int mouseX = -1;
        int mouseY = -1;

        
        // pseudo random number state
        mutable unsigned int state = 1804289383;

        const int bishopRelevantBits[64] = {
            6, 5, 5, 5, 5, 5, 5, 6, 
            5, 5, 5, 5, 5, 5, 5, 5, 
            5, 5, 7, 7, 7, 7, 5, 5, 
            5, 5, 7, 9, 9, 7, 5, 5, 
            5, 5, 7, 9, 9, 7, 5, 5, 
            5, 5, 7, 7, 7, 7, 5, 5, 
            5, 5, 5, 5, 5, 5, 5, 5, 
            6, 5, 5, 5, 5, 5, 5, 6
        };

        const int rookRelevantBits[64] = {
            12, 11, 11, 11, 11, 11, 11, 12, 
            11, 10, 10, 10, 10, 10, 10, 11, 
            11, 10, 10, 10, 10, 10, 10, 11, 
            11, 10, 10, 10, 10, 10, 10, 11, 
            11, 10, 10, 10, 10, 10, 10, 11, 
            11, 10, 10, 10, 10, 10, 10, 11, 
            11, 10, 10, 10, 10, 10, 10, 11, 
            12, 11, 11, 11, 11, 11, 11, 12
        };
};

#endif // CHESSBOARD_HPP