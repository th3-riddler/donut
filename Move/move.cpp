#include "move.hpp"
#include "../Macros/macros.hpp"

#include <iostream>


void Move::init() {
    initLeapersAttacks();
    initSlidingAttacks(Chessboard::bishop);
    initSlidingAttacks(Chessboard::rook);
    // initMagicNumbers();
    initPromotedPieces();
    // Search::clearTranspositionTable();
}

// Pseudo Random Number State
unsigned int randomState = 1804289383;

// Bishop's Relevant Bits
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

// Rook's Relevant Bits
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

// Rook Magic Numbers
const uint64_t rookMagicNumbers[64] = {
    0x8a80104000800020ULL, 
    0x140002000100040ULL, 
    0x2801880a0017001ULL, 
    0x100081001000420ULL, 
    0x200020010080420ULL, 
    0x3001c0002010008ULL, 
    0x8480008002000100ULL, 
    0x2080088004402900ULL, 
    0x800098204000ULL, 
    0x2024401000200040ULL, 
    0x100802000801000ULL, 
    0x120800800801000ULL, 
    0x208808088000400ULL, 
    0x2802200800400ULL, 
    0x2200800100020080ULL, 
    0x801000060821100ULL, 
    0x80044006422000ULL, 
    0x100808020004000ULL, 
    0x12108a0010204200ULL, 
    0x140848010000802ULL, 
    0x481828014002800ULL, 
    0x8094004002004100ULL, 
    0x4010040010010802ULL, 
    0x20008806104ULL, 
    0x100400080208000ULL, 
    0x2040002120081000ULL, 
    0x21200680100081ULL, 
    0x20100080080080ULL, 
    0x2000a00200410ULL, 
    0x20080800400ULL, 
    0x80088400100102ULL, 
    0x80004600042881ULL, 
    0x4040008040800020ULL, 
    0x440003000200801ULL, 
    0x4200011004500ULL, 
    0x188020010100100ULL, 
    0x14800401802800ULL, 
    0x2080040080800200ULL, 
    0x124080204001001ULL, 
    0x200046502000484ULL, 
    0x480400080088020ULL, 
    0x1000422010034000ULL, 
    0x30200100110040ULL, 
    0x100021010009ULL, 
    0x2002080100110004ULL, 
    0x202008004008002ULL, 
    0x20020004010100ULL, 
    0x2048440040820001ULL, 
    0x101002200408200ULL, 
    0x40802000401080ULL, 
    0x4008142004410100ULL, 
    0x2060820c0120200ULL, 
    0x1001004080100ULL, 
    0x20c020080040080ULL, 
    0x2935610830022400ULL, 
    0x44440041009200ULL, 
    0x280001040802101ULL, 
    0x2100190040002085ULL, 
    0x80c0084100102001ULL, 
    0x4024081001000421ULL, 
    0x20030a0244872ULL, 
    0x12001008414402ULL, 
    0x2006104900a0804ULL, 
    0x1004081002402ULL
};

// Bishop Magic Numbers
const uint64_t bishopMagicNumbers[64] = {
    0x40040844404084ULL, 
    0x2004208a004208ULL, 
    0x10190041080202ULL, 
    0x108060845042010ULL, 
    0x581104180800210ULL, 
    0x2112080446200010ULL, 
    0x1080820820060210ULL, 
    0x3c0808410220200ULL, 
    0x4050404440404ULL, 
    0x21001420088ULL, 
    0x24d0080801082102ULL, 
    0x1020a0a020400ULL, 
    0x40308200402ULL, 
    0x4011002100800ULL, 
    0x401484104104005ULL, 
    0x801010402020200ULL, 
    0x400210c3880100ULL, 
    0x404022024108200ULL, 
    0x810018200204102ULL, 
    0x4002801a02003ULL, 
    0x85040820080400ULL, 
    0x810102c808880400ULL, 
    0xe900410884800ULL, 
    0x8002020480840102ULL, 
    0x220200865090201ULL, 
    0x2010100a02021202ULL, 
    0x152048408022401ULL, 
    0x20080002081110ULL, 
    0x4001001021004000ULL, 
    0x800040400a011002ULL, 
    0xe4004081011002ULL, 
    0x1c004001012080ULL, 
    0x8004200962a00220ULL, 
    0x8422100208500202ULL, 
    0x2000402200300c08ULL, 
    0x8646020080080080ULL, 
    0x80020a0200100808ULL, 
    0x2010004880111000ULL, 
    0x623000a080011400ULL, 
    0x42008c0340209202ULL, 
    0x209188240001000ULL, 
    0x400408a884001800ULL, 
    0x110400a6080400ULL, 
    0x1840060a44020800ULL, 
    0x90080104000041ULL, 
    0x201011000808101ULL, 
    0x1a2208080504f080ULL, 
    0x8012020600211212ULL, 
    0x500861011240000ULL, 
    0x180806108200800ULL, 
    0x4000020e01040044ULL, 
    0x300000261044000aULL, 
    0x802241102020002ULL, 
    0x20906061210001ULL, 
    0x5a84841004010310ULL, 
    0x4010801011c04ULL, 
    0xa010109502200ULL, 
    0x4a02012000ULL, 
    0x500201010098b028ULL, 
    0x8040002811040900ULL, 
    0x28000010020204ULL, 
    0x6000020202d0240ULL, 
    0x8918844842082200ULL, 
    0x4010011029020020ULL
};

uint64_t Move::bishopMasks[64];
uint64_t Move::rookMasks[64];

uint64_t Move::bishopAttacks[64][512];
uint64_t Move::rookAttacks[64][4096];
uint64_t Move::pawnAttacks[2][64];
uint64_t Move::knightAttacks[64];
uint64_t Move::kingAttacks[64];
char Move::promotedPieces[11];

const int Move::castlingRightsMask[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11
};

void Move::initPromotedPieces() {
    promotedPieces[Chessboard::Q] = 'q';
    promotedPieces[Chessboard::R] = 'r';
    promotedPieces[Chessboard::B] = 'b';
    promotedPieces[Chessboard::N] = 'n';
    promotedPieces[Chessboard::q] = 'q';
    promotedPieces[Chessboard::r] = 'r';
    promotedPieces[Chessboard::b] = 'b';
    promotedPieces[Chessboard::n] = 'n';
}

// Get Bishop Attacks
uint64_t Move::getBishopAttacks(int square, uint64_t occupancy) {
    occupancy &= bishopMasks[square];
    occupancy *= bishopMagicNumbers[square];
    occupancy >>= 64 - bishopRelevantBits[square];

    return bishopAttacks[square][occupancy];
}

// Get Rook Attacks
uint64_t Move::getRookAttacks(int square, uint64_t occupancy) {
    occupancy &= rookMasks[square];
    occupancy *= rookMagicNumbers[square];
    occupancy >>= 64 - rookRelevantBits[square];

    return rookAttacks[square][occupancy];
}

// Get Queen Attacks
uint64_t Move::getQueenAttacks(int square, uint64_t occupancy) {
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}

// Mask Pawn Attacks
uint64_t Move::maskPawnAttacks(int square, int color) {
    uint64_t attacks = 0;
    uint64_t pos = 1ULL << square;

    if (color == Chessboard::white) {
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
uint64_t Move::maskKnightAttacks(int square) {
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
uint64_t Move::maskKingAttacks(int square) {
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
uint64_t Move::maskBishopAttacks(int square) {
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
uint64_t Move::maskRookAttacks(int square) {
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
uint64_t Move::bishopAttacksOnTheFly(int square, uint64_t block) {
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
uint64_t Move::rookAttacksOnTheFly(int square, uint64_t block) {
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

// Initialize Leaper Piece Attacks
void Move::initLeapersAttacks() {
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init pawn attacks
        pawnAttacks[Chessboard::white][square] = maskPawnAttacks(square, Chessboard::white);
        pawnAttacks[Chessboard::black][square] = maskPawnAttacks(square, Chessboard::black);
        
        // init knight attacks
        knightAttacks[square] = maskKnightAttacks(square);
        
        // init king attacks
        kingAttacks[square] = maskKingAttacks(square);
    }
}

// Initialize Sliding Piece Attacks
void Move::initSlidingAttacks(int bishop) {
    for (int square = 0; square < 64; square++) {
        bishopMasks[square] = maskBishopAttacks(square);
        rookMasks[square] = maskRookAttacks(square);

        uint64_t attackMask = (bishop ? bishopMasks[square] : rookMasks[square]);

        int relevantBitsCount = Chessboard::countBits(attackMask);
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
uint64_t Move::setOccupancy(int index, int bitsInMask, uint64_t attackMask) {
    uint64_t occupancy = 0ULL;

    for (int count = 0; count < bitsInMask; count++) {
        int square = Chessboard::getLSBIndex(attackMask);
        CLEAR_BIT(attackMask, square);
        if (index & (1 << count)) {
            occupancy |= (1ULL << square);
        }
    }

    return occupancy;
}

// Generate 32-bit pseudo-legal numbers
unsigned int Move::getRandomU32Number() {
    unsigned int number = randomState;

    // Xorshift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    randomState = number;

    return number;
}

// Generate 64-bit pseudo-legal numbers
uint64_t Move::getRandomU64Number() {
    uint64_t number1, number2, number3, number4;

    number1 = (uint64_t)(getRandomU32Number()) & 0xFFFF;
    number2 = (uint64_t)(getRandomU32Number()) & 0xFFFF;
    number3 = (uint64_t)(getRandomU32Number()) & 0xFFFF;
    number4 = (uint64_t)(getRandomU32Number()) & 0xFFFF;

    return number1 | (number2 << 16) | (number3 << 32) | (number4 << 48);
}

// Generate magic number candidate
uint64_t Move::generateMagicNumber() {
    return getRandomU64Number() & getRandomU64Number() & getRandomU64Number();
}

// Find appropriate magic number
uint64_t Move::findMagicNumber(int square, int relevantBits, int bishop) {
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
        if(Chessboard::countBits((attackMask * magicNumber) & 0xFF00000000000000) < 6) continue;

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
/*
    void Move::initMagicNumbers() {
        for (int square = 0; square < 64; square++) {
            std::cout << "0x" << std::hex << findMagicNumber(square, rookRelevantBits[square], Chessboard::rook) << "ULL, " << std::endl;
        }
        std::cout << std::endl;
        for (int square = 0; square < 64; square++) {
            std::cout << "0x" << std::hex << findMagicNumber(square, bishopRelevantBits[square], Chessboard::bishop) << "ULL, " << std::endl;
        }
    }
*/

// Print Move List
void Move::printMoveList(moves *moveList) {

    if (!moveList->count) {
        std::cout << "No moves found!" << std::endl;
        return;
    }

    std::cout << "\n    move    piece    capture    doublePush    enPassant    castling\n" << std::endl;

    for (int moveCount = 0; moveCount < moveList->count; moveCount++) {
        int move = moveList->moves[moveCount];
        std::cout << "    " << Chessboard::squareToCoordinates[getMoveSource(move)] <<
                               Chessboard::squareToCoordinates[getMoveTarget(move)] << 
                               (getMovePromoted(move) ? promotedPieces[getMovePromoted(move)] : ' ') << "     " <<
                               Chessboard::unicodePieces[getMovePiece(move)] << "         " << 
                               (getMoveCapture(move) != 13 ? Chessboard::unicodePieces[getMoveCapture(move)] : "0") << "           " << 
                               (getMoveDoublePush(move) ? 1 : 0) << "             " << 
                               (getMoveEnPassant(move) ? 1 : 0) << "           " << 
                               (getMoveCastling(move) ? 1 : 0) << std::endl;
    }
    std::cout << "\n\n  Total moves: " << moveList->count << std::endl;
}

// Print a Move (in UCI format)
void Move::printMove(int move) {
    if (promotedPieces[getMovePromoted(move)]) {
        std::cout << Chessboard::squareToCoordinates[getMoveSource(move)] << Chessboard::squareToCoordinates[getMoveTarget(move)] << promotedPieces[getMovePromoted(move)];
    } else {
        std::cout << Chessboard::squareToCoordinates[getMoveSource(move)] << Chessboard::squareToCoordinates[getMoveTarget(move)];
    }
}

// Print the score of each move
void Move::printMoveScores(moves *moveList) {
    for (int count = 0; count < moveList->count; count++) {
        printMove(moveList->moves[count]);
        std::cout << "   score: " << Evaluation::scoreMove(moveList->moves[count]) << std::endl;
    }
}

// Sort Moves
// void Move::sortMoves(moves *moveList) {
//     std::vector<std::pair<int, int>> scoredMoves;
//     for (int count = 0; count < moveList->count; count++) {
//         int score = Evaluation::scoreMove(moveList->moves[count]);
//         scoredMoves.emplace_back(score, moveList->moves[count]);
//     }
//     std::sort(scoredMoves.begin(), scoredMoves.end(), std::greater<>());

//     for (int count = 0; count < moveList->count; count++) {
//         moveList->moves[count] = scoredMoves[count].second;
//     }
// }

void Move::sortMoves(moves *moveList, int bestMove)
{
    // move scores
    int moveScores[moveList->count];

    for (int count = 0; count < moveList->count; count++) {
        if (bestMove == moveList->moves[count]) {
            moveScores[count] = 30000;
        }
        else {
            moveScores[count] = Evaluation::scoreMove(moveList->moves[count]);
        }
    }
    
    // loop over current move within a move list
    for (int currentMove = 0; currentMove < moveList->count; currentMove++)
    {
        // loop over next move within a move list
        for (int nextMove = currentMove + 1; nextMove < moveList->count; nextMove++)
        {
            // compare current and next move scores
            if (moveScores[currentMove] < moveScores[nextMove])
            {
                // swap scores
                int tempScore = moveScores[currentMove];
                moveScores[currentMove] = moveScores[nextMove];
                moveScores[nextMove] = tempScore;
                
                // swap moves
                int tempMove = moveList->moves[currentMove];
                moveList->moves[currentMove] = moveList->moves[nextMove];
                moveList->moves[nextMove] = tempMove;
            }
        }
    }
}
