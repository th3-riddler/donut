#include "chessboard.hpp"
#include "gameWindow.hpp"
#include "macros.hpp"

#include <SDL2/SDL_image.h>
#include <sstream>
#include <iostream>
#include<iomanip>

Chessboard::Chessboard(SDL_Renderer* renderer) : renderer(renderer) {
    std::cout << "Creata la Chessboard!" << std::endl;
    loadPieceTextures();
}

void Chessboard::drawGameState() {
    draw();
    drawPieces();
    if (dragging) {
        // print the position of the dragged piece
        drawDraggedPiece(this->draggedPiece);
    }
}

void Chessboard::drawDraggedPiece(PieceType pieceToDraw) {
    if (dragging && pieceToDraw != PIECE_TYPE_COUNT) {
        for (const auto& pair : pieceTextures) {
            std::cout << "Piece: " << pair.first << " Texture: " << pair.second << std::endl;
        }
        std::cout << "Texture to draw: " << pieceTextures[pieceTypeToChar.at(pieceToDraw)] << std::endl;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Rect dstRect = { mouseX - (squareSize / 2), mouseY - (squareSize / 2), squareSize, squareSize }; // Centra il pezzo sulla posizione del mouse

        SDL_RenderCopy(renderer, pieceTextures[pieceTypeToChar.at(pieceToDraw)], nullptr, &dstRect); // Disegna il pezzo trascinato alla posizione del mouse
    }
}

void Chessboard::draw() {
    // std::cout << "Chessboard::draw: chessboard address = " << this << std::endl;
    SDL_Rect square;
    square.w = squareSize;
    square.h = squareSize;

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if ((row + col) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 242, 225, 195, 255); // Light
            } else {
                SDL_SetRenderDrawColor(renderer, 195, 160, 130, 255); // Dark
            }
            square.x = col * squareSize;
            square.y = row * squareSize;
            SDL_RenderFillRect(renderer, &square);
        }
    }
}

void Chessboard::loadPieceTextures() {
    // std::cout << "Chessboard::loadTextures: chessboard address = " << this << std::endl;
    std::unordered_map<char, std::string> pieceFiles = {
        {'P', "images/wp.png"}, {'N', "images/wN.png"}, {'B', "images/wB.png"},
        {'R', "images/wR.png"}, {'Q', "images/wQ.png"}, {'K', "images/wK.png"},
        {'p', "images/bp.png"}, {'n', "images/bN.png"}, {'b', "images/bB.png"},
        {'r', "images/bR.png"}, {'q', "images/bQ.png"}, {'k', "images/bK.png"}
    };

    for (const auto& pair : pieceFiles) {
        SDL_Surface* surface = IMG_Load(pair.second.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << pair.second << " SDL_image Error: " << IMG_GetError() << std::endl;
            continue;
        }
        pieceTextures[pair.first] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    // printa le texture caricate
    // for (const auto& pair : pieceTextures) {
    //     std::cout << "Piece: " << pair.first << " Texture: " << pair.second << std::endl;
    // }
}

Chessboard::~Chessboard() {
    for (auto& pair : pieceTextures) {
        SDL_DestroyTexture(pair.second);
    }
}

void Chessboard::drawPieces() {
    // std::cout << "Chessboard::drawPieces: chessboard address = " << this << std::endl;
    auto drawPiece = [&](uint64_t bitboard, char piece) {
        for (int i = 0; i < 64; ++i) {
            if (bitboard & (1ULL << i)) {
                int x = (i % 8) * squareSize;
                int y = (7 - i / 8) * squareSize; // Flip y-axis for correct orientation
                SDL_Rect dstRect = {x, y, squareSize, squareSize};
                SDL_RenderCopy(renderer, pieceTextures[piece], nullptr, &dstRect);
            }
        }
    };

    // printBitboards();

    drawPiece(bitboard.bitboards[WHITE_PAWN], 'P');
    drawPiece(bitboard.bitboards[WHITE_KNIGHT], 'N');
    drawPiece(bitboard.bitboards[WHITE_BISHOP], 'B');
    drawPiece(bitboard.bitboards[WHITE_ROOK], 'R');
    drawPiece(bitboard.bitboards[WHITE_QUEEN], 'Q');
    drawPiece(bitboard.bitboards[WHITE_KING], 'K');
    drawPiece(bitboard.bitboards[BLACK_PAWN], 'p');
    drawPiece(bitboard.bitboards[BLACK_KNIGHT], 'n');
    drawPiece(bitboard.bitboards[BLACK_BISHOP], 'b');
    drawPiece(bitboard.bitboards[BLACK_ROOK], 'r');
    drawPiece(bitboard.bitboards[BLACK_QUEEN], 'q');
    drawPiece(bitboard.bitboards[BLACK_KING], 'k');
}

void Chessboard::initializeFromFEN(const std::string& fen) {
    parseFEN(fen);
}

void Chessboard::parseFEN(const std::string& fen) {
    std::istringstream iss(fen);
    std::string board, activeColor, castling, enPassant, halfMove, fullMove;

    iss >> board >> activeColor >> castling >> enPassant >> halfMove >> fullMove; // Read FEN components

    // Reset bitboards
    bitboard.bitboards = {};

    // Parse board
    int rank = 7;
    int file = 0;
    for (char c : board) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            file += c - '0';
        } else {
            uint64_t pos = 1ULL << (rank * 8 + file);
            switch (c) {
                case 'P': bitboard.bitboards[WHITE_PAWN] |= pos; break;
                case 'N': bitboard.bitboards[WHITE_KNIGHT] |= pos; break;
                case 'B': bitboard.bitboards[WHITE_BISHOP] |= pos; break;
                case 'R': bitboard.bitboards[WHITE_ROOK] |= pos; break;
                case 'Q': bitboard.bitboards[WHITE_QUEEN] |= pos; break;
                case 'K': bitboard.bitboards[WHITE_KING] |= pos; break;
                case 'p': bitboard.bitboards[BLACK_PAWN] |= pos; break;
                case 'n': bitboard.bitboards[BLACK_KNIGHT] |= pos; break;
                case 'b': bitboard.bitboards[BLACK_BISHOP] |= pos; break;
                case 'r': bitboard.bitboards[BLACK_ROOK] |= pos; break;
                case 'q': bitboard.bitboards[BLACK_QUEEN] |= pos; break;
                case 'k': bitboard.bitboards[BLACK_KING] |= pos; break;
            }
            file++;
        }
    }
    // std::cout << "Chessboard::FEN: chessboard address = " << this << std::endl;
    // printBitboards();

    // Parse active color
    whiteToMove = (activeColor == "w");

    // Parse castling rights
    bitboard.whiteKingCastle = (castling.find('K') != std::string::npos);
    bitboard.whiteQueenCastle = (castling.find('Q') != std::string::npos);
    bitboard.blackKingCastle = (castling.find('k') != std::string::npos);
    bitboard.blackQueenCastle = (castling.find('q') != std::string::npos);

    // Parse en passant square
    if (enPassant != "-") {
        int file = enPassant[0] - 'a'; 
        int rank = enPassant[1] - '1';
        bitboard.enPassantSquare = 1ULL << (rank * 8 + file);
    } else {
        bitboard.enPassantSquare = 0;
    }

    // Parse half move clock and full move counter
    bitboard.halfMoveClock = std::stoi(halfMove);
    bitboard.fullMoveCounter = std::stoi(fullMove);
}

void Chessboard::handleEvent(const SDL_Event& e) {
    // std::cout << "Chessboard::handleEvent: chessboard address = " << this << std::endl;
    // printBitboards();
    PieceType draggedPiece;
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // Convert mouse position to board coordinates
        int file = mouseX / squareSize;
        int rank = mouseY / squareSize;

        std::cout << "Clicked square: Rank --> " << rank << ", File --> " << file << std::endl;

        // Check if a piece is at the clicked position
        draggedPiece = getPieceAt(file, rank);
        std::cout << "Piece: " << draggedPiece << std::endl;
        if ((whiteToMove && draggedPiece < PIECE_TYPE_COUNT / 2) || (!whiteToMove && draggedPiece >= PIECE_TYPE_COUNT / 2 && draggedPiece < PIECE_TYPE_COUNT)) { // Check if the piece belongs to the player whose turn it is
            dragging = true;
            draggedPieceStartX = file;
            draggedPieceStartY = rank;
            this->draggedPiece = draggedPiece;

            // temporarily remove the piece from the board
            bitboard.bitboards[draggedPiece] &= ~(1ULL << ((7 - draggedPieceStartX) * 8 + draggedPieceStartY));
        }
    }
    if (e.type == SDL_MOUSEMOTION && dragging) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // Update the position of the dragged piece
        draggedPieceX = mouseX;
        draggedPieceY = mouseY;
    }
    if (e.type == SDL_MOUSEBUTTONUP && dragging) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // Convert mouse position to board coordinates
        int file = mouseX / squareSize;
        int rank = mouseY / squareSize;

        if (file == draggedPieceStartX && rank == draggedPieceStartY) {
            // Put the piece back on the board
            bitboard.bitboards[this->draggedPiece] |= 1ULL << ((7 - rank) * 8 + file);
        } else {
            // Create a move object and validate the move
            // Move move(draggedPieceStartX, draggedPieceStartY, file, rank, static_cast<PieceType>(draggedPiece));
            movePiece(draggedPieceStartX, draggedPieceStartY, file, rank, this->draggedPiece);
        }

        // // Drop the piece and update the board state
        // bitboard.movePiece(draggedPieceStartX, draggedPieceStartY, file, rank, static_cast<PieceType>(draggedPiece));

        dragging = false;
        this->draggedPiece = PIECE_TYPE_COUNT;
        draggedPieceStartX = -1;
        draggedPieceStartY = -1;
        draggedPieceX = -1;
        draggedPieceY = -1;
    }
}

PieceType Chessboard::getPieceAt(int file, int rank) const {
    printBitboards();

    uint64_t pos = 1ULL << ((7 - rank) * 8 + file);
    std::cout << "Position: " << pos << std::endl;

    for (const auto& pair : bitboard.bitboards) {
        std::cout << "Checking PieceType: " << pair.first << " Bitboard: " << pair.second << std::endl;
        if (pair.second & pos) {
            std::cout << "Found PieceType: " << pair.first << " at position: " << pos << std::endl;
            return pair.first;
        }
    }
    return PIECE_TYPE_COUNT;
}

void Chessboard::printBitboards() const {
    for (const auto& pair : bitboard.bitboards) {
        std::cout << "PieceType: " << pair.first << " Bitboard: 0x" 
                  << std::hex << std::setfill('0') << std::setw(16) << pair.second 
                  << std::dec << std::endl;
    }
}



std::vector<Move> Chessboard::generateMoves() const {
    std::vector<Move> moves;

    return moves;
}

uint64_t Chessboard::maskPawnAttacks(int square, bool isWhite) const {
    uint64_t attacks = 0;
    uint64_t pos = 1ULL << square;

    if (isWhite) {
        if (square % 8 != 0) {
            attacks |= pos << 7;
        }
        if (square % 8 != 7) {
            attacks |= pos << 9;
        }
    } else {
        if (square % 8 != 0) {
            attacks |= pos >> 9;
        }
        if (square % 8 != 7) {
            attacks |= pos >> 7;
        }
    }

    return attacks;
}


void Chessboard::movePiece(int startFile, int startRank, int endFile, int endRank, PieceType piece) {
    std::cout << "\nMoving piece: " << piece << " from (" << startFile << ", " << startRank << ") to (" << endFile << ", " << endRank << ")" << std::endl;

    // Verifica che le coordinate siano valide
    if (startFile < 0 || startFile >= 8 || startRank < 0 || startRank >= 8 ||
        endFile < 0 || endFile >= 8 || endRank < 0 || endRank >= 8) {
        std::cerr << "Invalid move coordinates!" << std::endl;
        return;
    }

    int startPosition = (7 - startRank) * 8 + startFile;
    int endPosition = (7 - endRank) * 8 + endFile;

    // Rimuovi il pezzo dalla posizione di partenza
    if (piece != PIECE_TYPE_COUNT) {
        for (const auto& pair : bitboard.bitboards) {
            if (pair.second & (1ULL << endPosition)) {
                bitboard.bitboards[pair.first] &= ~(1ULL << endPosition); // Rimuovi il pezzo avversario
                break;
            }
        }
        //bitboards[piece] &= ~(1ULL << startPosition); // Rimuovi il pezzo
        
        bitboard.bitboards[piece] |= (1ULL << endPosition);    // Posiziona il pezzo

        whiteToMove = !whiteToMove;
        std::cout << "Piece moved successfully!" << std::endl;
        std::cout << "White to move: " << whiteToMove << std::endl;
    }
}