#include "chessboard.hpp"

/*
TODO:
- Change the way occupancies bitboards are updated (change them in the same time the pieces are moved and not regenerating them every time)
- Change the generateMoves function to generate all legal moves in one go
*/

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

bool Chessboard::quit = false;
int Chessboard::movesToGo = 30;
int Chessboard::moveTime = -1;
int Chessboard::time = -1;
int Chessboard::inc = 0;
int Chessboard::startTime = 0;
int Chessboard::stopTime = 0;
int Chessboard::timeSet = 0;
bool Chessboard::stopped = false;
BitBoard Chessboard::bitboard;
uint64_t Chessboard::nodes;

uint64_t Chessboard::pieceKeys[12][64];
uint64_t Chessboard::enPassantKeys[64];
uint64_t Chessboard::castleKeys[16];
uint64_t Chessboard::sideKey;
uint64_t Chessboard::hashKey = 0ULL;


void Chessboard::init() {

    initCharPieces();
    initRandomKeys();

    init_nnue("nn-62ef826d1a6d.nnue");

    Search::initHashTable(64); // Default value of 64MB

    bool debug = false;

    if (debug) {
        parseFEN(startPosition);
        printBoard();

        std::cout << "Score from FEN: " << evaluate_fen_nnue(startPosition) << std::endl;

        int pieces[33];
        int squares[33];

        Evaluation::nnueInput(pieces, squares);
        int score = evaluate_nnue(bitboard.sideToMove, pieces, squares);
        std::cout << "Score direct: " << score << std::endl;

        int evalScore = Evaluation::evaluate();
        std::cout << "Eval score: " << evalScore << std::endl;

        // int start = getTimeMs();
        // Search::searchPosition(10);
        // std::cout << "Time: " << getTimeMs() - start << "ms" << std::endl;
    }
    else {
        uciLoop();
    }
    
}

void Chessboard::initRandomKeys() {

    for (int piece = P; piece <= k; piece++) {
        for (int square = 0; square < 64; square++) {
            pieceKeys[piece][square] = Move::getRandomU64Number();
        }
    }

    for (int square = 0; square < 64; square++) {
        enPassantKeys[square] = Move::getRandomU64Number();
    }

    for (int index = 0; index < 16; index++) {
        castleKeys[index] = Move::getRandomU64Number();
    }

    sideKey = Move::getRandomU64Number();
}

uint64_t Chessboard::generateHashKey() {
    uint64_t finalKey = 0ULL;
    uint64_t tempBitboard;

    for (int piece = P; piece <= k; piece++) {
        tempBitboard = bitboard.bitboards[piece];

        while (tempBitboard) {
            int square = getLSBIndex(tempBitboard);

            finalKey ^= pieceKeys[piece][square];

            CLEAR_BIT(tempBitboard, square);
        }
    }

    if (bitboard.enPassantSquare != noSquare) {
        finalKey ^= enPassantKeys[bitboard.enPassantSquare];
    }

    finalKey ^= castleKeys[bitboard.castlingRights];

    if (bitboard.sideToMove == black) {
        finalKey ^= sideKey;
    }

    return finalKey;
}

void Chessboard::resetBoard() {
    // Reset board position and state variables
    memset(bitboard.bitboards, 0ULL, sizeof(bitboard.bitboards));
    memset(bitboard.occupancies, 0ULL, sizeof(bitboard.occupancies));

    bitboard.sideToMove = 0;
    bitboard.enPassantSquare = Chessboard::noSquare;
    bitboard.castlingRights = 0;

    Search::repetitionIndex = 0;

    Search::fifty = 0;

    memset(Search::repetitionTable, 0ULL, sizeof(Search::repetitionTable));
}

// Parse the FEN string
void Chessboard::parseFEN(char *fen) {

    resetBoard();

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

    // Initialize hash key
    hashKey = generateHashKey();
}

// Print the given bitboard
void Chessboard::printBitboards(uint64_t bitboard) {
    std::cout << std::endl;
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

// Print the current board position
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

    std::cout << "Side to move:         " << (!bitboard.sideToMove ? "White" : "Black") << std::endl;

    std::cout << "En passant square:    " << (bitboard.enPassantSquare != noSquare ? squareToCoordinates[bitboard.enPassantSquare] : "no") << std::endl;

    std::cout << "Castling:             " << (bitboard.castlingRights & wk ? "K" : "-") << (bitboard.castlingRights & wq ? "Q" : "-") << (bitboard.castlingRights & bk ? "k" : "-") << (bitboard.castlingRights & bq ? "q" : "-") << std::endl;

    std::cout << "Hash Key:             " << std::hex << hashKey << std::dec << std::endl;

    std::cout << "Fifty:                " << std::hex << Search::fifty << std::dec << std::endl;

    std::cout << std::endl;
}


/*
               Binary Move Bits                                         Hexidecimal Constants                          

        000 0000 0000 0000 0000 0011 1111 --> source square                 0x3F
        000 0000 0000 0000 1111 1100 0000 --> target square                 0xFC0
        000 0000 0000 1111 0000 0000 0000 --> piece                         0xF000
        000 0000 1111 0000 0000 0000 0000 --> promoted piece                0xF0000
        000 1111 0000 0000 0000 0000 0000 --> captured piece                0xF00000
        001 0000 0000 0000 0000 0000 0000 --> double pawn push flag         0x1000000
        010 0000 0000 0000 0000 0000 0000 --> en passant flag               0x2000000
        100 0000 0000 0000 0000 0000 0000 --> castling flag                 0x4000000
*/

// Generate all moves
void Chessboard::generateMoves(moves *moveList) {
    moveList->count = 0;

    int sourceSquare, targetSquare, targetPiece = -1, startPiece, endPiece;

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
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, Q, 13, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, R, 13, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, B, 13, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, N, 13, 0, 0, 0));
                        }
                        else {
                            // One square pawn advance
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 13, 0, 0, 0));

                            // Double square pawn advance
                            if ((sourceSquare >= a2 && sourceSquare <= h2) && !GET_BIT(bitboard.occupancies[both], targetSquare + 8)) {
                                Move::addMove(moveList, encodeMove(sourceSquare, targetSquare + 8, piece, 0, 13, 1, 0, 0));
                            }
                        }
                    }

                    // Generate Capture Pawn Moves
                    attacks = Move::pawnAttacks[white][sourceSquare] & bitboard.occupancies[black];

                    while (attacks) {
                        targetSquare = getLSBIndex(attacks);

                        for (int bbPiece = p; bbPiece <= k; bbPiece++) {
                            if (GET_BIT(Chessboard::bitboard.bitboards[bbPiece], targetSquare)) {
                                targetPiece = bbPiece;
                                break;
                            }
                        }

                        // Pawn Promotion-Capture
                        if (sourceSquare >= a7 && sourceSquare <= h7) {
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, Q, targetPiece, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, R, targetPiece, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, B, targetPiece, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, N, targetPiece, 0, 0, 0));
                        }
                        else {
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, targetPiece, 0, 0, 0));
                        }

                        CLEAR_BIT(attacks, targetSquare);
                    }

                    // En passant captures
                    if (bitboard.enPassantSquare != noSquare) {
                        uint64_t enPassantAttacks = Move::pawnAttacks[white][sourceSquare] & (1ULL << bitboard.enPassantSquare);

                        if (enPassantAttacks) {
                            int enPassantTarget = getLSBIndex(enPassantAttacks);
                            Move::addMove(moveList, encodeMove(sourceSquare, enPassantTarget, piece, 0, p, 0, 1, 0));
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
                            Move::addMove(moveList, encodeMove(e1, g1, piece, 0, 13, 0, 0, 1));
                        }
                    }
                }

                // Queen Side Castling
                if (bitboard.castlingRights & wq) {
                    // Check if the squares between the king and rook are empty
                    if (!GET_BIT(bitboard.occupancies[both], d1) && !GET_BIT(bitboard.occupancies[both], c1) && !GET_BIT(bitboard.occupancies[both], b1)) {
                        // Check if the squares the king moves through are not under attack
                        if (!isSquareAttacked(e1, black) && !isSquareAttacked(d1, black)) {
                            Move::addMove(moveList, encodeMove(e1, c1, piece, 0, 13, 0, 0, 1));
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
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, q, 13, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, r, 13, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, b, 13, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, n, 13, 0, 0, 0));
                        }
                        else {
                            // One square pawn advance
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 13, 0, 0, 0));

                            // Double square pawn advance
                            if ((sourceSquare >= a7 && sourceSquare <= h7) && !GET_BIT(bitboard.occupancies[both], targetSquare - 8)) {
                                Move::addMove(moveList, encodeMove(sourceSquare, targetSquare - 8, piece, 0, 13, 1, 0, 0));
                            }
                        }
                    }

                    // Generate Capture Pawn Moves
                    attacks = Move::pawnAttacks[black][sourceSquare] & bitboard.occupancies[white];

                    while (attacks) {
                        targetSquare = getLSBIndex(attacks);

                        for (int bbPiece = P; bbPiece <= K; bbPiece++) {
                            if (GET_BIT(bitboard.bitboards[bbPiece], targetSquare)) {
                                targetPiece = bbPiece;
                                break;
                            }
                        }

                        // Pawn Promotion-Capture
                        if (sourceSquare >= a2 && sourceSquare <= h2) {
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, q, targetPiece, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, r, targetPiece, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, b, targetPiece, 0, 0, 0));
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, n, targetPiece, 0, 0, 0));
                        }
                        else {
                            Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, targetPiece, 0, 0, 0));
                        }

                        CLEAR_BIT(attacks, targetSquare);
                    }

                    // En passant captures
                    if (bitboard.enPassantSquare != noSquare) {
                        uint64_t enPassantAttacks = Move::pawnAttacks[black][sourceSquare] & (1ULL << bitboard.enPassantSquare);

                        if (enPassantAttacks) {
                            int enPassantTarget = getLSBIndex(enPassantAttacks);
                            Move::addMove(moveList, encodeMove(sourceSquare, enPassantTarget, piece, 0, P, 0, 1, 0));
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
                            Move::addMove(moveList, encodeMove(e8, g8, piece, 0, 13, 0, 0, 1));
                        }
                    }
                }

                // Queen Side Castling
                if (bitboard.castlingRights & bq) {
                    // Check if the squares between the king and rook are empty
                    if (!GET_BIT(bitboard.occupancies[both], d8) && !GET_BIT(bitboard.occupancies[both], c8) && !GET_BIT(bitboard.occupancies[both], b8)) {
                        // Check if the squares the king moves through are not under attack
                        if (!isSquareAttacked(e8, white) && !isSquareAttacked(d8, white)) {
                            Move::addMove(moveList, encodeMove(e8, c8, piece, 0, 13, 0, 0, 1));
                        }
                    }
                }
            }
        }

        // Generate Knight Moves
        if ((bitboard.sideToMove == white) ? piece == N : piece == n) {
            
            if (bitboard.sideToMove == white) { startPiece = p; endPiece = k; }
            else { startPiece = P; endPiece = K; }
            
            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::knightAttacks[sourceSquare] & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
                        if (GET_BIT(bitboard.bitboards[bbPiece], targetSquare)) {
                            targetPiece = bbPiece;
                            break;
                        }
                    }

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 13, 0, 0, 0));
                    else
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, targetPiece, 0, 0, 0));

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate Bishop Moves
        if ((bitboard.sideToMove == white) ? piece == B : piece == b) {

            if (bitboard.sideToMove == white) { startPiece = p; endPiece = k; }
            else { startPiece = P; endPiece = K; }

            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::getBishopAttacks(sourceSquare, bitboard.occupancies[both]) & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
                        if (GET_BIT(bitboard.bitboards[bbPiece], targetSquare)) {
                            targetPiece = bbPiece;
                            break;
                        }
                    }

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 13, 0, 0, 0));
                    else
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, targetPiece, 0, 0, 0));

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate Rook Moves
        if ((bitboard.sideToMove == white) ? piece == R : piece == r) {

            if (bitboard.sideToMove == white) { startPiece = Chessboard::p; endPiece = Chessboard::k; }
            else { startPiece = Chessboard::P; endPiece = Chessboard::K; }
            
            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::getRookAttacks(sourceSquare, bitboard.occupancies[both]) & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
                        if (GET_BIT(bitboard.bitboards[bbPiece], targetSquare)) {
                            targetPiece = bbPiece;
                            break;
                        }
                    }

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 13, 0, 0, 0));
                    else
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, targetPiece, 0, 0, 0));

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate Queen Moves
        if ((bitboard.sideToMove == white) ? piece == Q : piece == q) {

            if (bitboard.sideToMove == white) { startPiece = p; endPiece = k; }
            else { startPiece = P; endPiece = K; }

            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::getQueenAttacks(sourceSquare, bitboard.occupancies[both]) & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
                        if (GET_BIT(bitboard.bitboards[bbPiece], targetSquare)) {
                            targetPiece = bbPiece;
                            break;
                        }
                    }

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 13, 0, 0, 0));
                    else
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, targetPiece, 0, 0, 0));

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }

        // Generate King Moves
        if ((bitboard.sideToMove == white) ? piece == K : piece == k) {

            if (bitboard.sideToMove == white) { startPiece = p; endPiece = k; }
            else { startPiece = P; endPiece = K; }

            while (bitboardCopy) {
                sourceSquare = getLSBIndex(bitboardCopy);

                attacks = Move::kingAttacks[sourceSquare] & ((bitboard.sideToMove == white) ? ~bitboard.occupancies[white] : ~bitboard.occupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
                        if (GET_BIT(bitboard.bitboards[bbPiece], targetSquare)) {
                            targetPiece = bbPiece;
                            break;
                        }
                    }

                    if (!GET_BIT(((bitboard.sideToMove == white) ? bitboard.occupancies[black] : bitboard.occupancies[white]), targetSquare))
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 13, 0, 0, 0));
                    else
                        Move::addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, targetPiece, 0, 0, 0));

                    CLEAR_BIT(attacks, targetSquare);
                }

                CLEAR_BIT(bitboardCopy, sourceSquare);
            }
        }
    }
}

// Make a Move on the board
int Chessboard::makeMove(int move, int moveFlag) {
    // Quite Moves
    if (moveFlag == allMoves) {
        copyBoard();

        int sourceSquare = getMoveSource(move);
        int targetSquare = getMoveTarget(move);
        int piece = getMovePiece(move);
        int promotedPiece = getMovePromoted(move);

        CLEAR_BIT(bitboard.bitboards[piece], sourceSquare);
        SET_BIT(bitboard.bitboards[piece], targetSquare);

        hashKey ^= pieceKeys[piece][sourceSquare] ^ pieceKeys[piece][targetSquare];

        Search::fifty++;

        if (piece == P || piece == p) {
            Search::fifty = 0;
        }

        if (getMoveCapture(move) != 13) {
            Search::fifty = 0;
            int startPiece, endPiece;

            if (bitboard.sideToMove == white) {
                startPiece = p;
                endPiece = k;
            }
            else {
                startPiece = P;
                endPiece = K;
            }

            for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
                if (GET_BIT(bitboard.bitboards[bbPiece], targetSquare)) {
                    CLEAR_BIT(bitboard.bitboards[bbPiece], targetSquare);
                    hashKey ^= pieceKeys[bbPiece][targetSquare];
                    break;
                }
            }
        }

        if (promotedPiece) {
            CLEAR_BIT(bitboard.bitboards[piece], targetSquare);
            SET_BIT(bitboard.bitboards[promotedPiece], targetSquare);
            hashKey ^= pieceKeys[piece][targetSquare] ^ pieceKeys[promotedPiece][targetSquare];
        }

        if (getMoveEnPassant(move)) {
            // CLEAR_BIT(bitboard.bitboards[((bitboard.sideToMove == white) ? p : P)], targetSquare + ((bitboard.sideToMove == white) ? -8 : 8));
            if (bitboard.sideToMove == white) {
                CLEAR_BIT(bitboard.bitboards[p], targetSquare - 8);
                hashKey ^= pieceKeys[p][targetSquare - 8];
            }
            else {
                CLEAR_BIT(bitboard.bitboards[P], targetSquare + 8);
                hashKey ^= pieceKeys[P][targetSquare + 8];
            }
        }
        
        if (bitboard.enPassantSquare != noSquare) {
            hashKey ^= enPassantKeys[bitboard.enPassantSquare];
        }

        bitboard.enPassantSquare = noSquare;

        if (getMoveDoublePush(move)) {
            // bitboard.enPassantSquare = targetSquare + ((bitboard.sideToMove == white) ? -8 : 8);
            if (bitboard.sideToMove == white) {
                bitboard.enPassantSquare = targetSquare - 8;
                hashKey ^= enPassantKeys[targetSquare - 8];
            }
            else {
                bitboard.enPassantSquare = targetSquare + 8;
                hashKey ^= enPassantKeys[targetSquare + 8];
            }
        }

        if (getMoveCastling(move)) {
            switch (targetSquare) {
                case (g1):
                    CLEAR_BIT(bitboard.bitboards[R], h1);
                    SET_BIT(bitboard.bitboards[R], f1);
                    hashKey ^= pieceKeys[R][h1] ^ pieceKeys[R][f1];
                    break;
                case (c1):
                    CLEAR_BIT(bitboard.bitboards[R], a1);
                    SET_BIT(bitboard.bitboards[R], d1);
                    hashKey ^= pieceKeys[R][a1] ^ pieceKeys[R][d1];
                    break;
                case (g8):
                    CLEAR_BIT(bitboard.bitboards[r], h8);
                    SET_BIT(bitboard.bitboards[r], f8);
                    hashKey ^= pieceKeys[r][h8] ^ pieceKeys[r][f8];
                    break;
                case (c8):
                    CLEAR_BIT(bitboard.bitboards[r], a8);
                    SET_BIT(bitboard.bitboards[r], d8);
                    hashKey ^= pieceKeys[r][a8] ^ pieceKeys[r][d8];
                    break;
            }
        }

        hashKey ^= castleKeys[bitboard.castlingRights];
        bitboard.castlingRights &= Move::castlingRightsMask[sourceSquare] & Move::castlingRightsMask[targetSquare];
        hashKey ^= castleKeys[bitboard.castlingRights];

        memset(bitboard.occupancies, 0ULL, 24);

        for (int bbPiece = P; bbPiece <= K; bbPiece++) {
            bitboard.occupancies[white] |= bitboard.bitboards[bbPiece];
        }
        for (int bbPiece = p; bbPiece <= k; bbPiece++) {
            bitboard.occupancies[black] |= bitboard.bitboards[bbPiece];
        }
        bitboard.occupancies[both] = bitboard.occupancies[white] | bitboard.occupancies[black];

        bitboard.sideToMove ^= 1; // Switch side to move
        hashKey ^= sideKey;

        // ================ Debug Hash Key ================
        // uint64_t hashFromScratch = generateHashKey();

        // if (hashKey != hashFromScratch) {
        //     std::cout << "\nMAKE MOVE\n" << std::endl;
        //     std::cout << "move: ";
        //     Move::printMove(move);
        //     std::cout << std::endl;
        //     printBoard();
        //     std::cout << "Hash Key should be: " << std::hex << hashFromScratch << std::dec << std::endl;
        //     getchar();
        // }

        if (isSquareAttacked(getLSBIndex(((bitboard.sideToMove == white) ? bitboard.bitboards[k] : bitboard.bitboards[K])), bitboard.sideToMove)) {
            takeBack();
            return 0;
        }
        else {
            return 1;
        }
    }
    else { // Capture Moves
        if (getMoveCapture(move) != 13) {
            return makeMove(move, allMoves);
        }
        else {
            return 0;
        }
    }
    std::cout << "Invalid Move!" << std::endl;
    return 0;
}

// Detect if the given square is under attack by the given color
bool Chessboard::isSquareAttacked(int square, int side) {
    
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

int Chessboard::getTimeMs() {
    struct timeval timeValue;
    gettimeofday(&timeValue, NULL);
    return timeValue.tv_sec * 1000 + timeValue.tv_usec / 1000;
}

inline void Chessboard::perftDriver(int depth) {
    if (depth == 0) {
        nodes++;
        return;
    }

    moves moveList[1];
    generateMoves(moveList);

    for (int moveCount = 0; moveCount < moveList->count; moveCount++) {

        copyBoard();

        if (!makeMove(moveList->moves[moveCount], allMoves)) {
            continue; 
        }

        perftDriver(depth - 1);
        takeBack();

        // uint64_t hashFromScratch = generateHashKey();

        // if (hashKey != hashFromScratch) {
        //     std::cout << "\nTAKE BACK\n" << std::endl;
        //     std::cout << "move: ";
        //     Move::printMove(moveList->moves[moveCount]);
        //     std::cout << std::endl;
        //     printBoard();
        //     std::cout << "Hash Key should be: " << std::hex << hashFromScratch << std::dec << std::endl; 
        //     getchar();
        // }
    }
}

void Chessboard::perftTest(int depth) {
    std::cout << "\n    Performance Test\n" << std::endl;

    moves moveList[1];
    generateMoves(moveList);

    int start = getTimeMs();
    for (int moveCount = 0; moveCount < moveList->count; moveCount++) {

        copyBoard();

        if (!makeMove(moveList->moves[moveCount], allMoves)) {
            continue; 
        }

        long commulativeNodes = nodes;
        
        perftDriver(depth - 1);

        long oldNodes = nodes - commulativeNodes;

        takeBack();

        std::cout << "  Move: ";
        std::cout << squareToCoordinates[getMoveSource(moveList->moves[moveCount])] << squareToCoordinates[getMoveTarget(moveList->moves[moveCount])] << Move::promotedPieces[getMovePromoted(moveList->moves[moveCount])] << 
                     "  Nodes: " << oldNodes << std::endl;
    }
    std::cout << std::endl;
    std::cout << "  Depth: " << depth << std::endl <<
                 "  Nodes: " << nodes << std::endl <<
                 "  Time: " << getTimeMs() - start << "ms" << std::endl << std::endl;
}

int Chessboard::parseMove(char *moveString) {

    moves moveList[1];
    generateMoves(moveList);

    int sourceSquare = (moveString[0] - 'a') + (7 - (8 - (moveString[1] - '0'))) * 8;
    int targetSquare = (moveString[2] - 'a') + (7 - (8 - (moveString[3] - '0'))) * 8;

    for (int moveCount = 0; moveCount < moveList->count; moveCount++) {
        int move = moveList->moves[moveCount];

        if (sourceSquare == getMoveSource(move) && targetSquare == getMoveTarget(move)) {
            int promotedPiece = getMovePromoted(move);

            if (promotedPiece) {
                if (moveString[4] == Move::promotedPieces[promotedPiece]) {
                    return move;
                }
                continue;
            }
            return move;
        }
    }
    return 0;
}

void Chessboard::parsePosition(char *command) {
    command += 9;
    char *currentChar = command;

    // Parse "startpos" command
    if(strncmp(command, "startpos", 8) == 0) {
        // Initialize board to starting position
        parseFEN(startPosition);
    }
    else {
        // Parse FEN string
        currentChar = strstr(command, "fen");

        if (currentChar == NULL) {
            parseFEN(startPosition);
        }
        else {
            currentChar += 4;
            parseFEN(currentChar);
        }
    }

    // Parse moves
    currentChar = strstr(command, "moves");

    if (currentChar != NULL) {
        currentChar += 6;

        while(*currentChar) {
            int move = parseMove(currentChar);

            if (move == 0) {
                break;
            }

            Search::repetitionIndex++;
            Search::repetitionTable[Search::repetitionIndex] = hashKey;

            makeMove(move, allMoves);

            while (*currentChar && *currentChar != ' ') {
                currentChar++;
            }
            currentChar++;
        }
    }

    printBoard();
}

void Chessboard::resetTimeControl() {
    quit = false;
    movesToGo = 30;
    moveTime = -1;
    time = -1;
    inc = 0;
    startTime = 0;
    stopTime = 0;
    timeSet = false;
    stopped = false;
}

void Chessboard::parseGo(char *command) {

    resetTimeControl();

    int depth = -1;
    char *argument = NULL;

    if ((argument = strstr(command, "infinite"))) {}

    if ((argument = strstr(command, "binc")) && bitboard.sideToMove == black) {
        inc = atoi(argument + 5);
    }

    if ((argument = strstr(command, "winc")) && bitboard.sideToMove == white) {
        inc = atoi(argument + 5);
    }

    if ((argument = strstr(command, "wtime")) && bitboard.sideToMove == white) {
        time = atoi(argument + 6);
    }

    if ((argument = strstr(command, "btime")) && bitboard.sideToMove == black) {
        time = atoi(argument + 6);
    }

    if ((argument = strstr(command, "movestogo"))) {
        movesToGo = atoi(argument + 10);
    }

    if ((argument = strstr(command, "movetime"))) {
        moveTime = atoi(argument + 9);
    }
    
    if ((argument = strstr(command, "depth"))) {
        depth = atoi(argument + 6);
    }
    
    if (moveTime != -1) {
        time = moveTime;
        time -= 50;
        movesToGo = 1;
    }

    startTime = getTimeMs();

    depth = depth;

    if (time != -1) {
        timeSet = true;

        time /= movesToGo;

        time -= 50;

        if (time < 0) {

            time = 0;

            inc -= 50;

            if (inc < 0) { inc = 1; }
        }

        stopTime = startTime + time + inc;

    }

    if (depth == -1) {
        depth = 64;
    }

    std::cout << "time:" << time << " inc:" << inc << " start:" << startTime << " stop:" << stopTime << " depth:" << depth << " timeset:" << timeSet << std::endl;

    Search::searchPosition(depth);
}

int Chessboard::inputWaiting() {
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(fileno(stdin), &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select(16, &readfds, 0, 0, &tv);

    return (FD_ISSET(fileno(stdin), &readfds));
}

void Chessboard::readInput() {
    int bytes;
    char input[256] = "", *endc;

    if (inputWaiting()) {
        stopped = true;

        do {
            bytes = read(fileno(stdin), input, 256);
        }
        while (bytes < 0);

        endc = strchr(input, '\n');

        if (endc) {
            *endc = 0;
        }

        if (strlen(input) > 0) {
            if (!strncmp(input, "quit", 4)) {
                quit = true;
            }
        }
        else if (!strncmp(input, "stop", 4)) {
            quit = true;
        }
    }
}

void Chessboard::communicate() {
    if (timeSet == true && getTimeMs() > stopTime) {
        stopped = true;
    }

    readInput();
}

/*
    GUI Commands
        - "uci" --> "uciok"
        - "isready" --> "readyok"
        - "ucinewgame"
    
*/

void Chessboard::uciLoop() {

    int maxHash = 128;
    int mb = 64;

    // Reset STDIN and STDOUT buffers
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // Define User / GUI input buffer
    char input[2000];

    // Print Engine Information
    std::cout << "id name Iris " << version << std::endl;
    std::cout << "id author Redux" << std::endl;
    std::cout << "option name Hash type spin default 64 min 4 max " << maxHash << std::endl;
    std::cout << "uciok" << std::endl;

    while (true) {
        // Reset input buffer
        memset(input, 0, sizeof(input));
        fflush(stdout);

        if(!fgets(input, 2000, stdin)) {
            continue;
        }

        if (input[0] == '\n') {
            continue;
        }

        if (strncmp(input, "isready", 7) == 0) {
            std::cout << "readyok" << std::endl;
            continue;
        }

        else if (strncmp(input, "position", 8) == 0) {
            parsePosition(input);
            Search::clearTranspositionTable();
        }

        else if (strncmp(input, "ucinewgame", 10) == 0) {
            parsePosition("position startpos");
            Search::clearTranspositionTable();
        }

        else if (strncmp(input, "go", 2) == 0) {
            parseGo(input);
        }

        else if (strncmp(input, "quit", 4) == 0) {
            break;
        }

        else if (strncmp(input, "uci", 3) == 0) {
            std::cout << "id name Iris " << version << std::endl;
            std::cout << "id author Redux" << std::endl;
            std::cout << "uciok" << std::endl;
        }

        else if (!strncmp(input, "setoption name Hash value ", 26)) {
            sscanf(input, "%*s %*s %*s %*s %d", &mb);
            if (mb < 4) { mb = 4; }
            if (mb > maxHash) { mb = maxHash; }

            std::cout << "    Set hash table size to " << mb << "MB" << std::endl;
            Search::initHashTable(mb);
        }
    }
}