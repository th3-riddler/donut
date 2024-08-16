#include "chessboard.hpp"
#include "gameWindow.hpp"
#include "macros.hpp"
#include "preComputedMoveData.hpp"

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

        for (const auto& move : moves) {
            // std::cout << "Move: " << move.fromFile << ", " << move.fromRank << " --> " << move.toFile << ", " << move.toRank << std::endl;
            // highlightSquares(move.fromFile, move.fromRank, move.toFile, move.toRank);

            if (move.fromFile == draggedPieceStartX && move.fromRank == draggedPieceStartY) {
                // std::cout << "Coloring square: " << move.toFile << ", " << move.toRank << std::endl;
                colorSquare(move.toFile, move.toRank, 150, 0, 0);
            }
        }

        // print the position of the dragged piece
        drawDraggedPiece(this->draggedPiece);
    }
}

void Chessboard::drawDraggedPiece(int pieceToDraw) {
    if (dragging && pieceToDraw != Piece::None) {
        
        // std::cout << "Texture to draw: " << pieceTextures[Piece::getSymbol(pieceToDraw)] << std::endl;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Rect dstRect = { mouseX - (squareSize / 2), mouseY - (squareSize / 2), squareSize, squareSize }; // Centra il pezzo sulla posizione del mouse

        SDL_RenderCopy(renderer, pieceTextures[Piece::getSymbol(pieceToDraw)], nullptr, &dstRect); // Disegna il pezzo trascinato alla posizione del mouse
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

    drawPiece(bitboard.whitePawns, 'P');
    drawPiece(bitboard.whiteKnights, 'N');
    drawPiece(bitboard.whiteBishops, 'B');
    drawPiece(bitboard.whiteRooks, 'R');
    drawPiece(bitboard.whiteQueens, 'Q');
    drawPiece(bitboard.whiteKing, 'K');

    drawPiece(bitboard.blackPawns, 'p');
    drawPiece(bitboard.blackKnights, 'n');
    drawPiece(bitboard.blackBishops, 'b');
    drawPiece(bitboard.blackRooks, 'r');
    drawPiece(bitboard.blackQueens, 'q');
    drawPiece(bitboard.blackKing, 'k');
}

void Chessboard::initializeFromFEN(const std::string& fen) {
    parseFEN(fen);
}

void Chessboard::parseFEN(const std::string& fen) {
    std::istringstream iss(fen);
    std::string board, activeColor, castling, enPassant, halfMove, fullMove;

    iss >> board >> activeColor >> castling >> enPassant >> halfMove >> fullMove; // Read FEN components

    // Reset bitboards
    // bitboard.bitboards = {};

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
                case 'P': bitboard.whitePawns |= pos; break;
                case 'N': bitboard.whiteKnights |= pos; break;
                case 'B': bitboard.whiteBishops |= pos; break;
                case 'R': bitboard.whiteRooks |= pos; break;
                case 'Q': bitboard.whiteQueens |= pos; break;
                case 'K': bitboard.whiteKing |= pos; break;
                case 'p': bitboard.blackPawns |= pos; break;
                case 'n': bitboard.blackKnights |= pos; break;
                case 'b': bitboard.blackBishops |= pos; break;
                case 'r': bitboard.blackRooks |= pos; break;
                case 'q': bitboard.blackQueens |= pos; break;
                case 'k': bitboard.blackKing |= pos; break;
            }
            file++;
        }
    }
    // std::cout << "Chessboard::FEN: chessboard address = " << this << std::endl;
    // printBitboards();

    // Parse active color
    sideToMove = activeColor == "w" ? 0 : 1;

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

void Chessboard::colorSquare(int file, int rank, int r, int g, int b) {
    SDL_Rect square = {file * squareSize, rank * squareSize, squareSize, squareSize};
    SDL_SetRenderDrawColor(renderer, r, g, b, 100);
    SDL_RenderFillRect(renderer, &square);
}

void Chessboard::handleEvent(const SDL_Event& e) {
    // std::cout << "Chessboard::handleEvent: chessboard address = " << this << std::endl;
    // printBitboards();
    int draggedPiece;
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // Convert mouse position to board coordinates
        int file = mouseX / squareSize;
        int rank = mouseY / squareSize;

        std::cout << "Clicked square: Rank --> " << rank << ", File --> " << file << std::endl;

        // Check if a piece is at the clicked position
        draggedPiece = getPieceAt(file, rank);
        std::cout << "Piece: " << draggedPiece << std::endl;

        if ((Piece::isColor(draggedPiece, sideToMove)) && draggedPiece != Piece::None) { // Check if the piece belongs to the player whose turn it is
            dragging = true;
            draggedPieceStartX = file;
            draggedPieceStartY = rank;
            this->draggedPiece = draggedPiece;

            // temporarily remove the piece from the board
            *bitboard.bitboards[this->draggedPiece] &= ~(1ULL << ((7 - rank) * 8 + file));
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


        if ((file == draggedPieceStartX && rank == draggedPieceStartY) || !isInValidMoveList(file, rank, this->draggedPiece)) {
            // Put the piece back on the board
            std::cout << "Putting piece back on the board..." << std::endl;
            *bitboard.bitboards[this->draggedPiece] |= 1ULL << ((7 - draggedPieceStartY) * 8 + draggedPieceStartX);
        } else {
            // Create a move object and validate the move
            // Move move(draggedPieceStartX, draggedPieceStartY, file, rank, static_cast<PieceType>(draggedPiece));
            std::cout << "Moving piece..." << std::endl;
            movePiece(draggedPieceStartX, draggedPieceStartY, file, rank, this->draggedPiece);

            std::cout << "Generating pseudo-legal moves..." << std::endl;
            // Generate pseudo-legal moves
            moves = generatePseudoLegalMoves();
        }

        dragging = false;
        this->draggedPiece = 0;
        draggedPieceStartX = -1;
        draggedPieceStartY = -1;
        draggedPieceX = -1;
        draggedPieceY = -1;

        // // Drop the piece and update the board state
        // bitboard.movePiece(draggedPieceStartX, draggedPieceStartY, file, rank, static_cast<PieceType>(draggedPiece));

        
    }
}

bool Chessboard::isInValidMoveList(int file, int rank, int piece) {
    for (const auto& move : moves) {
        if (move.piece == piece && move.fromFile == draggedPieceStartX && move.fromRank == draggedPieceStartY && move.toFile == file && move.toRank == rank) {
            std::cout << "Valid move!" << std::endl;
            return true;
        }
    }
    std::cout << "Invalid move!" << std::endl;
    return false;
}

int Chessboard::getPieceAt(int file, int rank) const {
    // printBitboards();

    uint64_t pos = 1ULL << ((7 - rank) * 8 + file);
    // std::cout << "Position: " << pos << std::endl;

    for (const auto& pair : bitboard.bitboards) {
        // std::cout << "Checking Piece: " << pair.first << " Bitboard: " << pair.second << std::endl;
        if (*pair.second & pos) {
            // std::cout << "Found Piece: " << pair.first << " at position: " << pos << std::endl;
            return pair.first;
        }
    }
    return Piece::None;
}

void Chessboard::printBitboards() const {
    for (const auto& pair : bitboard.bitboards) {
        std::cout << "Piece: " << pair.first << " Bitboard: 0x" 
                  << std::hex << std::setfill('0') << std::setw(16) << *pair.second 
                  << std::dec << std::endl;
    }
}


std::vector<Move> Chessboard::generatePseudoLegalMoves() {
    moves.clear();

    // std::cout << "Generating pseudo-legal moves..." << std::endl;

    for (int startSquare = 0; startSquare < 64; ++startSquare) {
        // std::cout << "Start square: " << startSquare << std::endl;
        // std::cout << "Coordinate: " << startSquare % 8 << ", " << startSquare / 8 << std::endl;
        int piece = getPieceAt(startSquare % 8, startSquare / 8);
        // std::cout << "Piece: " << piece << std::endl;

        if (piece != Piece::None){
            // std::cout << "Entered!\n" << std::endl;
            if (Piece::isColor(piece, sideToMove)) {
                if (Piece::isSlidingPiece(piece)) {
                    // Generate sliding piece moves
                    generateSlidingMoves(startSquare, piece);
                } else {
                    // Generate non-sliding piece moves
                }
            }
        }
    }

    std::cout << moves.size() << std::endl;

    // print the moves
    // for (const auto& move : moves) {
    //     std::cout << "Move: " << move.fromFile << ", " << move.fromRank << " --> " << move.toFile << ", " << move.toRank << std::endl;
    // }

    return moves;
}


void Chessboard::generateSlidingMoves(int startSquare, int piece) {

    int startDirIndex = (Piece::getType(piece) == Piece::Bishop) ? 4 : 0;
    int endDirIndex = (Piece::getType(piece) == Piece::Rook) ? 4 : 8;

    for (int directionIndex = startDirIndex; directionIndex < endDirIndex; ++directionIndex) {
        for (int n = 0; n < preComputedMoveData::numSquaresToEdge[startSquare][directionIndex]; ++n) {
            int targetSquare = startSquare + preComputedMoveData::directionOffsets[directionIndex] * (n + 1);
            int targetPiece = getPieceAt(targetSquare % 8, targetSquare / 8);

            // Blocked by friendly piece
            if (Piece::isColor(targetPiece, sideToMove)) {
                // std::cout << "Blocked by friendly piece!" << std::endl;
                break;
            }

            moves.push_back(Move(startSquare % 8, startSquare / 8, targetSquare % 8, targetSquare / 8, piece));
            std::cout << "Move: " << startSquare % 8 << ", " << startSquare / 8 << " --> " << targetSquare % 8 << ", " << targetSquare / 8 << std::endl;

            // Blocked by enemy piece
            if (Piece::isColor(targetPiece, sideToMove ^ 8)) {
                // std::cout << "Blocked by enemy piece!" << std::endl;
                break;
            }
        }
    }
}

// std::vector<Move> Chessboard::generateMoves() {
//     // std::vector<Move> moves;

//     //return moves;
// }

void Chessboard::getPawnMoves(int square, int piece, std::vector<Move>& moves) {
    // Check if the pawn is white or black
    bool isWhite = Piece::isWhite(piece);

    // Check if the pawn is on the starting rank
    bool onStartingRank = (isWhite && square / 8 == 1) || (!isWhite && square / 8 == 6);

    // Check if the pawn is on the promotion rank
    bool onPromotionRank = (isWhite && square / 8 == 6) || (!isWhite && square / 8 == 1);
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


void Chessboard::movePiece(int startFile, int startRank, int endFile, int endRank, int piece) {
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
    if (piece != Piece::None) {
        for (const auto& pair : bitboard.bitboards) {
            if (*pair.second & (1ULL << endPosition)) {
                *bitboard.bitboards[pair.first] &= ~(1ULL << endPosition); // Rimuovi il pezzo avversario
                break;
            }
        }
        //bitboards[piece] &= ~(1ULL << startPosition); // Rimuovi il pezzo
        
        *bitboard.bitboards[piece] |= (1ULL << endPosition);    // Posiziona il pezzo

        //cambia il turno tra 0 e 8
        sideToMove ^= 8;
        std::cout << "Piece moved successfully!" << std::endl;
        std::cout << "White to move: " << sideToMove << std::endl;
    }
}