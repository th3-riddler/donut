#include "chessboard.hpp"
#include "gameWindow.hpp"
#include "macros.hpp"
#include "preComputedMoveData.hpp"

#include <SDL2/SDL_image.h>
#include <sstream>
#include <iostream>
#include<iomanip>

Chessboard::Chessboard(SDL_Renderer* renderer) : renderer(renderer) {
    initializeCharPieces();
    // loadPieceTextures();
}


// void Chessboard::drawGameState() {

//     SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Imposta la modalità di blending

//     draw();
    
//     if (dragging) {
//         colorSquare();
//     }

//     drawPieces();

//     if (dragging) {
//         // print the position of the dragged piece
//         drawDraggedPiece(this->draggedPiece);
//     }
// }

void Chessboard::colorSquare() {
    // Imposta il colore per le caselle evidenziate
    SDL_SetRenderDrawColor(renderer, 248, 52, 52, 150); // Usa un'opacità fissa

    // Disegna tutte le caselle evidenziate in un batch
    SDL_Rect dstRect = {0, 0, squareSize, squareSize};
    for (const auto& move : moves) {
        if (move.fromFile == draggedPieceStartX && move.fromRank == draggedPieceStartY) {
            dstRect.x = move.toFile * squareSize;
            dstRect.y = (7 - move.toRank) * squareSize;
            SDL_RenderFillRect(renderer, &dstRect);
        }
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

// void Chessboard::drawPieces() {
//     // std::cout << "Chessboard::drawPieces: chessboard address = " << this << std::endl;
//     auto drawPiece = [&](uint64_t bitboard, char piece) {
//         for (int i = 0; i < 64; ++i) {
//             if (bitboard & (1ULL << i)) {
//                 int x = (i % 8) * squareSize;
//                 int y = (7 - i / 8) * squareSize; // Flip y-axis for correct orientation
//                 SDL_Rect dstRect = {x, y, squareSize, squareSize};
//                 SDL_RenderCopy(renderer, pieceTextures[piece], nullptr, &dstRect);
//             }
//         }
//     };

//     // printBitboards();

//     drawPiece(bitboard.whitePawns, 'P');
//     drawPiece(bitboard.whiteKnights, 'N');
//     drawPiece(bitboard.whiteBishops, 'B');
//     drawPiece(bitboard.whiteRooks, 'R');
//     drawPiece(bitboard.whiteQueens, 'Q');
//     drawPiece(bitboard.whiteKing, 'K');

//     drawPiece(bitboard.blackPawns, 'p');
//     drawPiece(bitboard.blackKnights, 'n');
//     drawPiece(bitboard.blackBishops, 'b');
//     drawPiece(bitboard.blackRooks, 'r');
//     drawPiece(bitboard.blackQueens, 'q');
//     drawPiece(bitboard.blackKing, 'k');
// }

void Chessboard::parseFEN(const char *fen) {

    // Reset board position and state variables
    memset(bitboard.bitboards, 0ULL, sizeof(bitboard.bitboards));
    memset(bitboard.occupancies, 0ULL, sizeof(bitboard.occupancies));

    bitboard.sideToMove = 0;
    bitboard.enPassantSquare = Chessboard::noSquare;
    bitboard.castlingRights = 0;

    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){
            int square = (7 - rank) * 8 + file;

            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                int piece = charPieces[*fen];
                
                SET_BIT(bitboard.bitboards[piece], square);

                *fen++;
            }

            if (*fen >= '0' && *fen <= '9') {
                int offset = *fen - '0'; // Convert the character '0' to integer 0

                int piece = -1;

                for (int bbPiece = P; bbPiece <= k; bbPiece++) {
                    if (GET_BIT(bitboard.bitboards[bbPiece], square)) {
                        piece = bbPiece;
                    }
                }

                if (piece == -1)
                    file--;

                file += offset;
                *fen++;
            }

            if (*fen == '/') {
                *fen++;
            }
        }
    }
    // Parse side to move
    *fen++;
    bitboard.sideToMove = (*fen == 'w') ? white : black;

    // Parse castling rights
    fen += 2;
    while (*fen != ' ') {
        switch (*fen) {
            case 'K': bitboard.castlingRights |= wk; break;
            case 'Q': bitboard.castlingRights |= wq; break;
            case 'k': bitboard.castlingRights |= bk; break;
            case 'q': bitboard.castlingRights |= bq; break;
            case '-': break;
        }
        *fen++;
    }

    // Parse en passant square
    *fen++;

    if (*fen != '-') {
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');

        bitboard.enPassantSquare = (7 - rank) * 8 + file;
    }
    else {
        bitboard.enPassantSquare = Chessboard::noSquare;
    }


    // Initialize white occupancies
    for (int piece = P; piece <= K; piece++) {
        bitboard.occupancies[white] |= bitboard.bitboards[piece];
    }

    // Initialize black occupancies
    for (int piece = p; piece <= k; piece++) {
        bitboard.occupancies[black] |= bitboard.bitboards[piece];
    }

    // Initialize both occupancies
    bitboard.occupancies[both] = bitboard.occupancies[white] | bitboard.occupancies[black];


    std::cout << "FEN: " << fen << std::endl;
}

// void Chessboard::handleEvent(const SDL_Event& e) {
//     // std::cout << "Chessboard::handleEvent: chessboard address = " << this << std::endl;
//     // printBitboards();
//     int draggedPiece;
//     if (e.type == SDL_MOUSEBUTTONDOWN) {
//         SDL_GetMouseState(&mouseX, &mouseY);

//         // Convert mouse position to board coordinates
//         int file = mouseX / squareSize;
//         int rank = 7 - (mouseY / squareSize);

//         std::cout << "Clicked square: Rank --> " << rank << ", File --> " << file << std::endl;

//         // Check if a piece is at the clicked position
//         draggedPiece = getPieceAt(file, rank);
//         std::cout << "Piece: " << draggedPiece << std::endl;

//         if ((Piece::isColor(draggedPiece, sideToMove)) && draggedPiece != Piece::None) { // Check if the piece belongs to the player whose turn it is
//             dragging = true;
//             draggedPieceStartX = file;
//             draggedPieceStartY = rank;
//             this->draggedPiece = draggedPiece;

//             // temporarily remove the piece from the board
//             *bitboard.bitboards[this->draggedPiece] &= ~(1ULL << (rank * 8 + file));
//         }
//     }
//     if (e.type == SDL_MOUSEMOTION && dragging) {
//         SDL_GetMouseState(&mouseX, &mouseY);

//         // Update the position of the dragged piece
//         draggedPieceX = mouseX;
//         draggedPieceY = mouseY;
//     }
//     if (e.type == SDL_MOUSEBUTTONUP && dragging) {
//         SDL_GetMouseState(&mouseX, &mouseY);

//         // Convert mouse position to board coordinates
//         int file = mouseX / squareSize;
//         int rank = 7 - (mouseY / squareSize);


//         if ((file == draggedPieceStartX && rank == draggedPieceStartY) || !isInValidMoveList(file, rank, this->draggedPiece)) {
//             // Put the piece back on the board
//             std::cout << "Putting piece back on the board..." << std::endl;
//             *bitboard.bitboards[this->draggedPiece] |= 1ULL << (draggedPieceStartY * 8 + draggedPieceStartX);
//         } else {
//             // Create a move object and validate the move
//             // Move move(draggedPieceStartX, draggedPieceStartY, file, rank, static_cast<PieceType>(draggedPiece));
//             std::cout << "Moving piece..." << std::endl;
//             movePiece(draggedPieceStartX, draggedPieceStartY, file, rank, this->draggedPiece);

//             std::cout << "Generating pseudo-legal moves..." << std::endl;
//             // Generate pseudo-legal moves
//             moves = generatePseudoLegalMoves();
//         }

//         dragging = false;
//         this->draggedPiece = 0;
//         draggedPieceStartX = -1;
//         draggedPieceStartY = -1;
//         draggedPieceX = -1;
//         draggedPieceY = -1;
//     }
// }

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

// int Chessboard::getPieceAt(int file, int rank) const {
//     // printBitboards();

//     uint64_t pos = 1ULL << (rank * 8 + file);
//     // std::cout << "Position: " << pos << std::endl;

//     for (const auto& pair : bitboard.bitboards) {
//         // std::cout << "Checking Piece: " << pair.first << " Bitboard: " << pair.second << std::endl;
//         if (*pair.second & pos) {
//             // std::cout << "Found Piece: " << pair.first << " at position: " << pos << std::endl;
//             return pair.first;
//         }
//     }
//     return Piece::None;
// }

void Chessboard::printBitboards(uint64_t bitboard) const {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file; // Calcola la posizione del bit
            std::cout << ((bitboard & (1ULL << square)) ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Bitboard: " << bitboard << "\n" << std::endl;
}

void Chessboard::printBoard() const {
    std::cout << std::endl;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file;

            if (!file)
                std::cout << "  " << 8 - rank << "  ";

            int piece = -1;

            for (int bbPiece = P; bbPiece <= k; bbPiece++) {
                if (GET_BIT(bitboard.bitboards[bbPiece], square)) {
                    piece = bbPiece;
                }
            }

            if (piece == -1) {
                std::cout << ". ";
            }
            else {
                std::cout << unicodePieces[piece] << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "\n     a b c d e f g h\n" << std::endl;

    std::cout << "Side to move: " << (!bitboard.sideToMove ? "White" : "Black") << std::endl;

    std::cout << "En passant square: " << (bitboard.enPassantSquare != noSquare ? squareToCoordinates[bitboard.enPassantSquare] : "no") << std::endl;

    std::cout << "Castling: " << (bitboard.castlingRights & wk ? "K" : "-") << (bitboard.castlingRights & wq ? "Q" : "-") << (bitboard.castlingRights & bk ? "k" : "-") << (bitboard.castlingRights & bq ? "q" : "-") << std::endl;

    std::cout << std::endl;
}


// std::vector<Move> Chessboard::generatePseudoLegalMoves() {      //  >> SINISTRA (diminuisce, basso) || << DESTRA (aumenta, alto)
    
//     moves.clear();

//     // std::cout << "Generating pseudo-legal moves..." << std::endl;

//     for (int startSquare = 0; startSquare < 64; ++startSquare) {
//         int piece = getPieceAt(startSquare % 8, startSquare / 8);
//         // std::cout << "Piece: " << piece << std::endl;

//         if (piece != Piece::None){
//             // std::cout << "Entered!\n" << std::endl;
//             if (Piece::isColor(piece, sideToMove)) {
//                 if (Piece::isSlidingPiece(piece)) {
//                     // Generate sliding piece moves
//                     // generateSlidingMoves(startSquare, piece);
//                     getPawnMoves(startSquare, piece);
//                 } else {
//                     // generateLeapingMoves(startSquare, piece);
//                 }
//             }
//         }
//     }

//     std::cout << moves.size() << std::endl;

//     // print the moves
//     // for (const auto& move : moves) {
//     //     std::cout << "Move: " << move.fromFile << ", " << move.fromRank << " --> " << move.toFile << ", " << move.toRank << std::endl;
//     // }

//     return moves;
// }


// void Chessboard::generateLeapingMoves(int startSquare, int piece) {

//     if (Piece::getType(piece) == Piece::Pawn) {
//         getPawnMoves(startSquare, piece, moves);
//     } else if (Piece::getType(piece) == Piece::Knight) {
//     } else if (Piece::getType(piece) == Piece::King) {
//     }
// }

// void Chessboard::generateSlidingMoves(int startSquare, int piece) {

//     int startDirIndex = (Piece::getType(piece) == Piece::Bishop) ? 4 : 0;
//     int endDirIndex = (Piece::getType(piece) == Piece::Rook) ? 4 : 8;

//     for (int directionIndex = startDirIndex; directionIndex < endDirIndex; ++directionIndex) {
//         for (int n = 0; n < preComputedMoveData::numSquaresToEdge[startSquare][directionIndex]; ++n) {
//             int targetSquare = startSquare + preComputedMoveData::directionOffsets[directionIndex] * (n + 1);
//             int targetPiece = getPieceAt(targetSquare % 8, targetSquare / 8);

//             // Blocked by friendly piece
//             if (Piece::isColor(targetPiece, sideToMove)) {
//                 // std::cout << "Blocked by friendly piece!" << std::endl;
//                 break;
//             }

//             moves.push_back(Move(startSquare % 8, startSquare / 8, targetSquare % 8, targetSquare / 8, piece));
//             std::cout << "Move: " << startSquare % 8 << ", " << startSquare / 8 << " --> " << targetSquare % 8 << ", " << targetSquare / 8 << std::endl;

//             // Blocked by enemy piece
//             if (Piece::isColor(targetPiece, sideToMove ^ 8)) {
//                 // std::cout << "Blocked by enemy piece!" << std::endl;
//                 break;
//             }
//         }
//     }
// }

// std::vector<Move> Chessboard::generateMoves() {
//     // std::vector<Move> moves;

//     //return moves;
// }

void Chessboard::getPawnMoves() {

    initAll();

    uint64_t occupancy = 0ULL;
    SET_BIT(occupancy, d5);

    printBitboards(getRookAttacks(d4, occupancy));

    // for (int targetSquare = 0; targetSquare < 64; ++targetSquare) {
    //     if (attacks & (1ULL << targetSquare)) {
    //         moves.push_back(Move(square % 8, square / 8, targetSquare % 8, targetSquare / 8, piece));
    //     }
    // }
}

// Mask Pawn Attacks
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

// Mask Knight Attacks
uint64_t Chessboard::maskKnightAttacks(int square) const {
    uint64_t attacks = 0;
    uint64_t pos = 1ULL << square;

    // Due file in alto, una colonna a sinistra
    if (square % 8 > 0 && square / 8 < 6) { 
        attacks |= pos << 15;
    }
    // Due file in alto, una colonna a destra
    if (square % 8 < 7 && square / 8 < 6) {
        attacks |= pos << 17;
    }
    // Due file in basso, una colonna a sinistra
    if (square % 8 > 0 && square / 8 > 1) {
        attacks |= pos >> 17;
    }
    // Due file in basso, una colonna a destra
    if (square % 8 < 7 && square / 8 > 1) {
        attacks |= pos >> 15;
    }
    // Una fila in alto, due colonne a sinistra
    if (square % 8 > 1 && square / 8 < 7) {
        attacks |= pos << 6;
    }
    // Una fila in alto, due colonne a destra
    if (square % 8 < 6 && square / 8 < 7) {
        attacks |= pos << 10;
    }
    // Una fila in basso, due colonne a sinistra
    if (square % 8 > 1 && square / 8 > 0) {
        attacks |= pos >> 10;
    }
    // Una fila in basso, due colonne a destra
    if (square % 8 < 6 && square / 8 > 0) {
        attacks |= pos >> 6;
    }

    return attacks;
}

// Mask King Attacks
uint64_t Chessboard::maskKingAttacks(int square) const {
    uint64_t attacks = 0;
    uint64_t pos = 1ULL << square;

    if(square % 8 > 0) {
        attacks |= pos >> 1; // Sinistra
        attacks |= pos >> 9; // Sinistra basso
        attacks |= pos << 7; // Sinistra alto
    }
    if(square % 8 < 7) {
        attacks |= pos << 1; // Destra
        attacks |= pos << 9; // Destra basso
        attacks |= pos >> 7; // Destra alto
    }
    attacks |= pos >> 8; // Basso
    attacks |= pos << 8; // Alto

    return attacks;
}

// Mask Bishop Attacks (Bishop Relevant Occupancy Bits)
uint64_t Chessboard::maskBishopAttacks(int square) const {
    uint64_t attacks = 0;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    for (rank = targetRank + 1, file = targetFile + 1; rank <= 6 && file <= 6; rank++, file++) {
        attacks |= 1ULL << (rank * 8 + file);
    }
    
    for (rank = targetRank - 1, file = targetFile + 1; rank >= 1 && file <= 6; rank--, file++) {
        attacks |= 1ULL << (rank * 8 + file);
    }

    for (rank = targetRank + 1, file = targetFile - 1; rank <= 6 && file >= 1; rank++, file--) {
        attacks |= 1ULL << (rank * 8 + file);
    }

    for (rank = targetRank - 1, file = targetFile - 1; rank >= 1 && file >= 1; rank--, file--) {
        attacks |= 1ULL << (rank * 8 + file);
    }

    return attacks;
}

// Mask Rook Attacks (Rook Relevant Occupancy Bits)
uint64_t Chessboard::maskRookAttacks(int square) const {
    uint64_t attacks = 0;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    for (rank = targetRank + 1; rank <= 6; rank++) {
        attacks |= 1ULL << (rank * 8 + targetFile);
    }

    for (rank = targetRank - 1; rank >= 1; rank--) {
        attacks |= 1ULL << (rank * 8 + targetFile);
    }

    for (file = targetFile + 1; file <= 6; file++) {
        attacks |= 1ULL << (targetRank * 8 + file);
    }

    for (file = targetFile - 1; file >= 1; file--) {
        attacks |= 1ULL << (targetRank * 8 + file);
    }

    return attacks;
}

// Generate Bishop Attacks on the fly
uint64_t Chessboard::bishopAttacksOnTheFly(int square, uint64_t block) const {
    uint64_t attacks = 0;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    for (rank = targetRank + 1, file = targetFile + 1; rank <= 7 && file <= 7; rank++, file++) {
        attacks |= 1ULL << (rank * 8 + file);
        if (block & (1ULL << (rank * 8 + file))) break;
    }
    
    for (rank = targetRank - 1, file = targetFile + 1; rank >= 0 && file <= 7; rank--, file++) {
        attacks |= 1ULL << (rank * 8 + file);
        if (block & (1ULL << (rank * 8 + file))) break;
    }

    for (rank = targetRank + 1, file = targetFile - 1; rank <= 7 && file >= 0; rank++, file--) {
        attacks |= 1ULL << (rank * 8 + file);
        if (block & (1ULL << (rank * 8 + file))) break;
    }

    for (rank = targetRank - 1, file = targetFile - 1; rank >= 0 && file >= 0; rank--, file--) {
        attacks |= 1ULL << (rank * 8 + file);
        if (block & (1ULL << (rank * 8 + file))) break;
    }

    return attacks;
}

// Generate Rook Attacks on the fly
uint64_t Chessboard::rookAttacksOnTheFly(int square, uint64_t block) const {
    uint64_t attacks = 0;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    for (rank = targetRank + 1; rank <= 7; rank++) {
        attacks |= 1ULL << (rank * 8 + targetFile);
        if (block & (1ULL << (rank * 8 + targetFile))) break;
    }

    for (rank = targetRank - 1; rank >= 0; rank--) {
        attacks |= 1ULL << (rank * 8 + targetFile);
        if (block & (1ULL << (rank * 8 + targetFile))) break;
    }

    for (file = targetFile + 1; file <= 7; file++) {
        attacks |= 1ULL << (targetRank * 8 + file);
        if (block & (1ULL << (targetRank * 8 + file))) break;
    }

    for (file = targetFile - 1; file >= 0; file--) {
        attacks |= 1ULL << (targetRank * 8 + file);
        if (block & (1ULL << (targetRank * 8 + file))) break; 
    }

    return attacks;
}

void Chessboard::initAll() {
    initLeapersAttacks();
    initSlidingAttacks(bishop);
    initSlidingAttacks(rook);
}

// Initialize Leaper Piece Attacks
void Chessboard::initLeapersAttacks() {
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init pawn attacks
        pawnAttacks[white][square] = maskPawnAttacks(square, white);
        pawnAttacks[black][square] = maskPawnAttacks(square, black);
        
        // init knight attacks
        knightAttacks[square] = maskKnightAttacks(square);
        
        // init king attacks
        kingAttacks[square] = maskKingAttacks(square);
    }
}

// Initialize Sliding Piece Attacks
void Chessboard::initSlidingAttacks(int bishop) {
    for (int square = 0; square < 64; square++) {
        bishopMasks[square] = maskBishopAttacks(square);
        rookMasks[square] = maskRookAttacks(square);

        uint64_t attackMask = (bishop ? bishopMasks[square] : rookMasks[square]);

        int relevantBitsCount = countBits(attackMask);
        int occupancyIndices = (1 << relevantBitsCount);

        for (int index = 0; index < occupancyIndices; index++) {
            
            if (bishop) {
                uint64_t occupancy = setOccupancy(index, relevantBitsCount, bishopMasks[square]);
                int magicIndex = (int)((occupancy * bishopMagicNumbers[square]) >> (64 - bishopRelevantBits[square]));
                bishopAttacks[square][magicIndex] = bishopAttacksOnTheFly(square, occupancy);
            }
            else {
                uint64_t occupancy = setOccupancy(index, relevantBitsCount, rookMasks[square]);
                int magicIndex = (int)((occupancy * rookMagicNumbers[square]) >> (64 - rookRelevantBits[square]));
                rookAttacks[square][magicIndex] = rookAttacksOnTheFly(square, occupancy);
            }            
        }
    }
}


// Set occupancies
uint64_t Chessboard::setOccupancy(int index, int bitsInMask, uint64_t attackMask) const {
    uint64_t occupancy = 0ULL;

    for (int count = 0; count < bitsInMask; count++) {
        int square = getLSBIndex(attackMask);
        CLEAR_BIT(attackMask, square);
        if (index & (1 << count)) {
            occupancy |= (1ULL << square);
        }
    }

    return occupancy;
}

// Generate 32-bit pseudo-legal numbers
unsigned int Chessboard::getRandomU32Number() const {
    unsigned int number = randomState;

    // Xorshift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    randomState = number;

    return number;
}

// Generate 64-bit pseudo-legal numbers
uint64_t Chessboard::getRandomU64Number() const {
    uint64_t number1, number2, number3, number4;

    number1 = (uint64_t)(getRandomU32Number()) & 0xFFFF;
    number2 = (uint64_t)(getRandomU32Number()) & 0xFFFF;
    number3 = (uint64_t)(getRandomU32Number()) & 0xFFFF;
    number4 = (uint64_t)(getRandomU32Number()) & 0xFFFF;

    return number1 | (number2 << 16) | (number3 << 32) | (number4 << 48);
}

// Generate magic number candidate
uint64_t Chessboard::generateMagicNumber() const {
    return getRandomU64Number() & getRandomU64Number() & getRandomU64Number();
}

// Find appropriate magic number
uint64_t Chessboard::findMagicNumber(int square, int relevantBits, int bishop) const {
    uint64_t occupancies[4096];
    uint64_t attacks[4096];

    uint64_t usedAttacks[4096];
    uint64_t attackMask = bishop ? maskBishopAttacks(square) : maskRookAttacks(square);

    int occupancyIndices = 1 << relevantBits;

    for (int index = 0; index < occupancyIndices; index++) {
        occupancies[index] = setOccupancy(index, relevantBits, attackMask);
        attacks[index] = bishop ? bishopAttacksOnTheFly(square, occupancies[index]) : rookAttacksOnTheFly(square, occupancies[index]);
    }

    // Test Magic Numbers
    for (int randomCount = 0; randomCount < 100000000; randomCount++) {
        // Generate Magic Number Candidate
        uint64_t magicNumber = generateMagicNumber();

        // Skip inappropriate Magic Numbers
        if(countBits((attackMask * magicNumber) & 0xFF00000000000000) < 6) continue;

        // Initialize used attacks
        memset(usedAttacks, 0ULL, sizeof(usedAttacks));

        int index, fail;
        
        // Check if the magic number is appropriate
        for (index = 0, fail = 0; !fail && index < occupancyIndices; index++) {
            int magicIndex = (int)((occupancies[index] * magicNumber) >> (64 - relevantBits));

            if (usedAttacks[magicIndex] == 0ULL) {
                usedAttacks[magicIndex] = attacks[index];
            } else if (usedAttacks[magicIndex] != attacks[index]) {
                fail = 1;
            }
        }
        if (!fail)
            return magicNumber;
    }
    std::cout << "Magic Number not found!" << std::endl;
    return 0ULL;
}

// Initialize Magic Numbers
// void Chessboard::initMagicNumbers() {
//     for (int square = 0; square < 64; square++) {
//         std::cout << "0x" << std::hex << findMagicNumber(square, rookRelevantBits[square], rook) << "ULL, " << std::endl;
//     }
//     std::cout << std::endl;
//     for (int square = 0; square < 64; square++) {
//         std::cout << "0x" << std::hex << findMagicNumber(square, bishopRelevantBits[square], bishop) << "ULL, " << std::endl;
//     }
// }

// Get Bishop Attacks
uint64_t Chessboard::getBishopAttacks(int square, uint64_t occupancy) const {
    occupancy &= bishopMasks[square];
    occupancy *= bishopMagicNumbers[square];
    occupancy >>= 64 - bishopRelevantBits[square];

    return bishopAttacks[square][occupancy];
}

// Get Rook Attacks
uint64_t Chessboard::getRookAttacks(int square, uint64_t occupancy) const {
    occupancy &= rookMasks[square];
    occupancy *= rookMagicNumbers[square];
    occupancy >>= 64 - rookRelevantBits[square];

    return rookAttacks[square][occupancy];
}

uint64_t Chessboard::getQueenAttacks(int square, uint64_t occupancy) const {
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}

// void Chessboard::movePiece(int startFile, int startRank, int endFile, int endRank, int piece) {
//     std::cout << "\nMoving piece: " << piece << " from (" << startFile << ", " << startRank << ") to (" << endFile << ", " << endRank << ")" << std::endl;

//     // Verifica che le coordinate siano valide
//     if (startFile < 0 || startFile >= 8 || startRank < 0 || startRank >= 8 ||
//         endFile < 0 || endFile >= 8 || endRank < 0 || endRank >= 8) {
//         std::cerr << "Invalid move coordinates!" << std::endl;
//         return;
//     }

//     int startPosition = startRank * 8 + startFile;
//     int endPosition = endRank * 8 + endFile;

//     // Rimuovi il pezzo dalla posizione di partenza
//     if (piece != Piece::None) {
//         for (const auto& pair : bitboard.bitboards) {
//             if (*pair.second & (1ULL << endPosition)) {
//                 *bitboard.bitboards[pair.first] &= ~(1ULL << endPosition); // Rimuovi il pezzo avversario
//                 break;
//             }
//         }
//         //bitboards[piece] &= ~(1ULL << startPosition); // Rimuovi il pezzo
        
//         *bitboard.bitboards[piece] |= (1ULL << endPosition);    // Posiziona il pezzo

//         //cambia il turno tra 0 e 8
//         // sideToMove ^= 8;
//         std::cout << "Piece moved successfully!" << std::endl;
//         std::cout << "White to move: " << sideToMove << std::endl;
//     }
// }