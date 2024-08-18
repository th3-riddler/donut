#include "chessboard.hpp"
#include "gameWindow.hpp"
#include "macros.hpp"

#include <SDL2/SDL_image.h>
#include <sstream>
#include <iostream>
#include <iomanip>

// Chessboard::Chessboard(SDL_Renderer* renderer) : renderer(renderer) {
//     initializeCharPieces();
//     // loadPieceTextures();
// }

char Chessboard::asciiPieces[13] = "PNBRQKpnbrqk";
const char* Chessboard::squareToCoordinates[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};
const char* Chessboard::unicodePieces[13] = {
    "♙", "♘", "♗", "♖", "♕", "♔",
    "♟", "♞", "♝", "♜", "♛", "♚"
};

int Chessboard::charPieces[128] = {};
inline void Chessboard::initCharPieces() {
    charPieces['P'] = P;
    charPieces['N'] = N;
    charPieces['B'] = B;
    charPieces['R'] = R;
    charPieces['Q'] = Q;
    charPieces['K'] = K;
    charPieces['p'] = p;
    charPieces['n'] = n;
    charPieces['b'] = b;
    charPieces['r'] = r;
    charPieces['q'] = q;
    charPieces['k'] = k;
}

BitBoard Chessboard::bitboard;

void Chessboard::init() {

    initCharPieces();

    getPawnMoves();
    
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

// void Chessboard::colorSquare() {
//     // Imposta il colore per le caselle evidenziate
//     SDL_SetRenderDrawColor(renderer, 248, 52, 52, 150); // Usa un'opacità fissa

//     // Disegna tutte le caselle evidenziate in un batch
//     SDL_Rect dstRect = {0, 0, squareSize, squareSize};
//     for (const auto& move : moves) {
//         if (move.fromFile == draggedPieceStartX && move.fromRank == draggedPieceStartY) {
//             dstRect.x = move.toFile * squareSize;
//             dstRect.y = (7 - move.toRank) * squareSize;
//             SDL_RenderFillRect(renderer, &dstRect);
//         }
//     }
// }

// void Chessboard::drawDraggedPiece(int pieceToDraw) {
//     if (dragging && pieceToDraw != Piece::None) {
        
//         // std::cout << "Texture to draw: " << pieceTextures[Piece::getSymbol(pieceToDraw)] << std::endl;
//         int mouseX, mouseY;
//         SDL_GetMouseState(&mouseX, &mouseY);
//         SDL_Rect dstRect = { mouseX - (squareSize / 2), mouseY - (squareSize / 2), squareSize, squareSize }; // Centra il pezzo sulla posizione del mouse

//         SDL_RenderCopy(renderer, pieceTextures[Piece::getSymbol(pieceToDraw)], nullptr, &dstRect); // Disegna il pezzo trascinato alla posizione del mouse
//     }
// }

// void Chessboard::draw() {
//     // std::cout << "Chessboard::draw: chessboard address = " << this << std::endl;
//     SDL_Rect square;
//     square.w = squareSize;
//     square.h = squareSize;

//     for (int row = 0; row < boardSize; ++row) {
//         for (int col = 0; col < boardSize; ++col) {
//             if ((row + col) % 2 == 0) {
//                 SDL_SetRenderDrawColor(renderer, 242, 225, 195, 255); // Light
//             } else {
//                 SDL_SetRenderDrawColor(renderer, 195, 160, 130, 255); // Dark
//             }
//             square.x = col * squareSize;
//             square.y = row * squareSize;
//             SDL_RenderFillRect(renderer, &square);
//         }
//     }
// }

// void Chessboard::loadPieceTextures() {
//     // std::cout << "Chessboard::loadTextures: chessboard address = " << this << std::endl;
//     std::unordered_map<char, std::string> pieceFiles = {
//         {'P', "images/wp.png"}, {'N', "images/wN.png"}, {'B', "images/wB.png"},
//         {'R', "images/wR.png"}, {'Q', "images/wQ.png"}, {'K', "images/wK.png"},
//         {'p', "images/bp.png"}, {'n', "images/bN.png"}, {'b', "images/bB.png"},
//         {'r', "images/bR.png"}, {'q', "images/bQ.png"}, {'k', "images/bK.png"}
//     };

//     for (const auto& pair : pieceFiles) {
//         SDL_Surface* surface = IMG_Load(pair.second.c_str());
//         if (!surface) {
//             std::cerr << "Failed to load image: " << pair.second << " SDL_image Error: " << IMG_GetError() << std::endl;
//             continue;
//         }
//         pieceTextures[pair.first] = SDL_CreateTextureFromSurface(renderer, surface);
//         SDL_FreeSurface(surface);
//     }
//     // printa le texture caricate
//     // for (const auto& pair : pieceTextures) {
//     //     std::cout << "Piece: " << pair.first << " Texture: " << pair.second << std::endl;
//     // }
// }

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

                fen++;
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
                fen++;
            }

            if (*fen == '/') {
                fen++;
            }
        }
    }
    // Parse side to move
    fen++;
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
        fen++;
    }

    // Parse en passant square
    fen++;

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

// bool Chessboard::isInValidMoveList(int file, int rank, int piece) {
//     for (const auto& move : moves) {
//         if (move.piece == piece && move.fromFile == draggedPieceStartX && move.fromRank == draggedPieceStartY && move.toFile == file && move.toRank == rank) {
//             std::cout << "Valid move!" << std::endl;
//             return true;
//         }
//     }
//     std::cout << "Invalid move!" << std::endl;
//     return false;
// }

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

void Chessboard::printBitboards(uint64_t bitboard) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file; // Calcola la posizione del bit

            if (!file)
                std::cout << "  " << 8 - rank << "  ";
                
            std::cout << ((bitboard & (1ULL << square)) ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << "\n     a b c d e f g h\n" << std::endl;

    std::cout << "Bitboard: " << bitboard << "\n" << std::endl;
}

void Chessboard::printBoard() {
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

// Print a Move (in UCI format)
void Chessboard::printMove(int move) {
    std::cout << squareToCoordinates[getMoveSource(move)] << squareToCoordinates[getMoveTarget(move)] << Move::promotedPieces[getMovePromoted(move)] << std::endl;
}

// Print Move List
void Chessboard::printMoveList(moves *moveList) {

    std::cout << "\nmove    piece    capture    doublePush    enPassant    castling\n" << std::endl;

    for (int moveCount = 0; moveCount < moveList->count; moveCount++) {
        int move = moveList->moves[moveCount];
        std::cout << squareToCoordinates[getMoveSource(move)] <<
                     squareToCoordinates[getMoveTarget(move)] << 
                     Move::promotedPieces[getMovePromoted(move)] << "     " <<
                     unicodePieces[getMovePiece(move)] << "         " << 
                     (getMoveCapture(move) ? 1 : 0) << "           " << 
                     (getMoveDoublePush(move) ? 1 : 0) << "             " << 
                     (getMoveEnPassant(move) ? 1 : 0) << "           " << 
                     (getMoveCastling(move) ? 1 : 0) << std::endl;
    }
    std::cout << "\n\n  Total moves: " << moveList->count << std::endl;
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


/*
               Binary Move Bits                                         Hexidecimal Constants                          

        0000 0000 0000 0000 0011 1111 --> source square                 0x3F
        0000 0000 0000 1111 1100 0000 --> target square                 0xFC0
        0000 0000 1111 0000 0000 0000 --> piece                         0xF000
        0000 1111 0000 0000 0000 0000 --> promoted piece                0xF0000
        0001 0000 0000 0000 0000 0000 --> capture flag                  0x100000
        0010 0000 0000 0000 0000 0000 --> double pawn push flag         0x200000
        0100 0000 0000 0000 0000 0000 --> en passant flag               0x400000
        1000 0000 0000 0000 0000 0000 --> castling flag                 0x800000
*/



void Chessboard::getPawnMoves() {

    // initAll();

    moves moveList[1];
    moveList->count = 0;


    Move::addMove(moveList, encodeMove(d7, e8, P, Q, 1, 0, 0, 0));


    printMoveList(moveList);

    // uint64_t occupancy = 0ULL;
    // SET_BIT(occupancy, d7);
    // SET_BIT(occupancy, c4);

    // printBitboards(Move::getQueenAttacks(d5, occupancy));

    // for (int targetSquare = 0; targetSquare < 64; ++targetSquare) {
    //     if (attacks & (1ULL << targetSquare)) {
    //         moves.push_back(Move(square % 8, square / 8, targetSquare % 8, targetSquare / 8, piece));
    //     }
    // }
}

// Generate all moves
inline void Chessboard::generateMoves() {
    int sourceSquare, targetSquare;

    uint64_t bitboardCopy, attacks;

    for (int piece = P; piece <= k; piece++) {
        bitboardCopy = bitboard.bitboards[piece];

        // Generate White Pawn Moves and White King Castling Moves
        if (bitboard.sideToMove == white) {
            if (piece == P) {
                while (bitboardCopy) {
                    sourceSquare = getLSBIndex(bitboardCopy);
                    targetSquare = sourceSquare + 8;

                    // Generate Quite Pawn Moves
                    if (!(targetSquare > h8) && !GET_BIT(bitboard.occupancies[both], targetSquare)) {
                        // Pawn Promotion
                        if (sourceSquare >= a7 && sourceSquare <= h7) {
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "q" << std::endl;
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "r" << std::endl;
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "b" << std::endl;
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "n" << std::endl;
                        }
                        else {
                            // One square pawn advance
                            std::cout << "Pawn Push: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;

                            // Double square pawn advance
                            if ((sourceSquare >= a2 && sourceSquare <= h2) && !GET_BIT(bitboard.occupancies[both], targetSquare + 8)) {
                                std::cout << "Double Pawn Push: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare + 8] << std::endl;
                            }
                        }
                    }

                    // Generate Capture Pawn Moves
                    attacks = Move::pawnAttacks[white][sourceSquare] & bitboard.occupancies[black];

                    while (attacks) {
                        targetSquare = getLSBIndex(attacks);

                        // Pawn Promotion
                        if (sourceSquare >= a7 && sourceSquare <= h7) {
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "q" << std::endl;
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "r" << std::endl;
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "b" << std::endl;
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "n" << std::endl;
                        }
                        else {
                            std::cout << "Pawn Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;
                        }

                        CLEAR_BIT(attacks, targetSquare);
                    }

                    // En passant captures
                    if (bitboard.enPassantSquare != noSquare) {
                        uint64_t enPassantAttacks = Move::pawnAttacks[white][sourceSquare] & (1ULL << bitboard.enPassantSquare);

                        if (enPassantAttacks) {
                            int enPassantTarget = getLSBIndex(enPassantAttacks);

                            std::cout << "Pawn en Passant Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[enPassantTarget] << std::endl;
                        }
                    }

                    // Pop LSB from bitboardCopy
                    CLEAR_BIT(bitboardCopy, sourceSquare);
                }
            }

            // Castling Moves
            if (piece == K) {
                // King Side Castling
                if (bitboard.castlingRights & wk) {
                    // Check if the squares between the king and rook are empty
                    if (!GET_BIT(bitboard.occupancies[both], f1) && !GET_BIT(bitboard.occupancies[both], g1)) {
                        // Check if the squares the king moves through are not under attack
                        if (!isSquareAttacked(e1, black) && !isSquareAttacked(f1, black)) {
                            std::cout << "King Side Castling: e1g1" << std::endl;
                        }
                    }
                }

                // Queen Side Castling
                if (bitboard.castlingRights & wq) {
                    // Check if the squares between the king and rook are empty
                    if (!GET_BIT(bitboard.occupancies[both], d1) && !GET_BIT(bitboard.occupancies[both], c1) && !GET_BIT(bitboard.occupancies[both], b1)) {
                        // Check if the squares the king moves through are not under attack
                        if (!isSquareAttacked(e1, black) && !isSquareAttacked(d1, black)) {
                            std::cout << "Queen Side Castling: e1c1" << std::endl;
                        }
                    }
                }
            }
        }
        else { // Generate Black Pawn Moves and Black King Castling Moves
            if (piece == p) {
                while (bitboardCopy) {
                    sourceSquare = getLSBIndex(bitboardCopy);
                    targetSquare = sourceSquare - 8;

                    // Generate Quite Pawn Moves
                    if (!(targetSquare < a1) && !GET_BIT(bitboard.occupancies[both], targetSquare)) {
                        // Pawn Promotion
                        if (sourceSquare >= a2 && sourceSquare <= h2) {
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "q" << std::endl;
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "r" << std::endl;
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "b" << std::endl;
                            std::cout << "Pawn Promotion: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "n" << std::endl;
                        }
                        else {
                            // One square pawn advance
                            std::cout << "Pawn Push: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;

                            // Double square pawn advance
                            if ((sourceSquare >= a7 && sourceSquare <= h7) && !GET_BIT(bitboard.occupancies[both], targetSquare - 8)) {
                                std::cout << "Double Pawn Push: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare - 8] << std::endl;
                            }
                        }
                    }

                    // Generate Capture Pawn Moves
                    attacks = Move::pawnAttacks[black][sourceSquare] & bitboard.occupancies[white];

                    while (attacks) {
                        targetSquare = getLSBIndex(attacks);

                        // Pawn Promotion
                        if (sourceSquare >= a2 && sourceSquare <= h2) {
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "q" << std::endl;
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "r" << std::endl;
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "b" << std::endl;
                            std::cout << "Pawn Promotion-Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << "n" << std::endl;
                        }
                        else {
                            std::cout << "Pawn Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;
                        }

                        CLEAR_BIT(attacks, targetSquare);
                    }

                    // En passant captures
                    if (bitboard.enPassantSquare != noSquare) {
                        uint64_t enPassantAttacks = Move::pawnAttacks[black][sourceSquare] & (1ULL << bitboard.enPassantSquare);

                        if (enPassantAttacks) {
                            int enPassantTarget = getLSBIndex(enPassantAttacks);

                            std::cout << "Pawn en Passant Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[enPassantTarget] << std::endl;
                        }
                    }

                    // Pop LSB from bitboardCopy
                    CLEAR_BIT(bitboardCopy, sourceSquare);
                }
            }

            // Castling Moves
            if (piece == k) {
                // King Side Castling
                if (bitboard.castlingRights & bk) {
                    // Check if the squares between the king and rook are empty
                    if (!GET_BIT(bitboard.occupancies[both], f8) && !GET_BIT(bitboard.occupancies[both], g8)) {
                        // Check if the squares the king moves through are not under attack
                        if (!isSquareAttacked(e8, white) && !isSquareAttacked(f8, white)) {
                            std::cout << "King Side Castling: e8g8" << std::endl;
                        }
                    }
                }

                // Queen Side Castling
                if (bitboard.castlingRights & bq) {
                    // Check if the squares between the king and rook are empty
                    if (!GET_BIT(bitboard.occupancies[both], d8) && !GET_BIT(bitboard.occupancies[both], c8) && !GET_BIT(bitboard.occupancies[both], b8)) {
                        // Check if the squares the king moves through are not under attack
                        if (!isSquareAttacked(e8, white) && !isSquareAttacked(d8, white)) {
                            std::cout << "Queen Side Castling: e8c8" << std::endl;
                        }
                    }
                }
            }
        }

        // Generate Knight Moves
        if ((bitboard.sideToMove == white) ? piece == N : piece == n) {
            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::knightAttacks[sourceSquare] & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        std::cout << "Knight Move: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;
                    else
                        std::cout << "Knight Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate Bishop Moves
        if ((bitboard.sideToMove == white) ? piece == B : piece == b) {
            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::getBishopAttacks(sourceSquare, bitboard.occupancies[both]) & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        std::cout << "Bishop Move: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;
                    else
                        std::cout << "Bishop Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate Rook Moves
        if ((bitboard.sideToMove == white) ? piece == R : piece == r) {
            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::getRookAttacks(sourceSquare, bitboard.occupancies[both]) & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        std::cout << "Rook Move: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;
                    else
                        std::cout << "Rook Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate Queen Moves
        if ((bitboard.sideToMove == white) ? piece == Q : piece == q) {
            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::getQueenAttacks(sourceSquare, bitboard.occupancies[both]) & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        std::cout << "Queen Move: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;
                    else
                        std::cout << "Queen Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate King Moves
        if ((bitboard.sideToMove == white) ? piece == K : piece == k) {
            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::kingAttacks[sourceSquare] & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        std::cout << "King Move: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;
                    else
                        std::cout << "King Capture: " << squareToCoordinates[sourceSquare] << squareToCoordinates[targetSquare] << std::endl;

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }
    }
}

// Detect if the given square is under attack by the given color
inline bool Chessboard::isSquareAttacked(int square, int side) {
    
    // Attacked by White Pawns
    if ((side == white) && (Move::pawnAttacks[black][square] & bitboard.bitboards[P])) return true; 

    // Attacked by Black Pawns
    if ((side == black) && (Move::pawnAttacks[white][square] & bitboard.bitboards[p])) return true;

    // Attacked by Knights
    if ((Move::knightAttacks[square] & ((side == white) ? bitboard.bitboards[N] : bitboard.bitboards[n]))) return true;

    // Attacked by Kings
    if ((Move::kingAttacks[square] & ((side == white) ? bitboard.bitboards[K] : bitboard.bitboards[k]))) return true;

    // Attacked by Bishops
    if (Move::getBishopAttacks(square, bitboard.occupancies[both]) & ((side == white) ? bitboard.bitboards[B] : bitboard.bitboards[b])) return true;

    // Attacked by Rooks
    if (Move::getRookAttacks(square, bitboard.occupancies[both]) & ((side == white) ? bitboard.bitboards[R] : bitboard.bitboards[r])) return true;

    // Attacked by Queens
    if (Move::getQueenAttacks(square, bitboard.occupancies[both]) & ((side == white) ? bitboard.bitboards[Q] : bitboard.bitboards[q])) return true;
    
    return false;
}

// Print attacked squares
void Chessboard::printAttackedSquares(int side) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file;

            if (!file)
                std::cout << "  " << 8 - rank << "  ";
            
            std::cout << (isSquareAttacked(square, side) ? "X " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << "\n     a b c d e f g h\n" << std::endl;
}

// Initialize All
// void Chessboard::initAll() {
//     Move::initLeapersAttacks();
//     Move::initSlidingAttacks(bishop);
//     Move::initSlidingAttacks(rook);
// }


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