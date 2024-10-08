#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../Evaluation/evaluation.hpp"
#include "../Chessboard/chessboard.hpp"
#include "../nnueEval/nnueEval.hpp"
#include "../Macros/macros.hpp"
#include "../Move/move.hpp"
#include "../Reader/reader.hpp"

struct moves;

struct tt {
    uint64_t hashKey;
    int depth;
    int flags;
    int score;
    int bestMove;
};

class Search {
    public:
        static int negamax(int alpha, int beta, int depth);
        static int quiescenceSearch(int alpha, int beta);

        static void searchPosition(int depth);
        static inline void enablePvScore(moves *moveList);
        static void clearTranspositionTable();
        static int readHashEntry(int alpha, int beta, int* bestMove, int depth);
        static void writeHashEntry(int score, int bestMove, int depth, int flag);
        static inline bool isRepetition();
        static void initHashTable(int mb);

        static int ply;

        static Reader::BookMoves bookMoves;

        static int killerMoves[2][maxPly];
        static int historyMoves[12][64];
        static int pvLength[maxPly];
        static int pvTable[maxPly][maxPly];
        static bool followPv;
        static bool scorePv;
        static int fifty;

        static int hashEntries;

        static const int fullDepthMoves;
        static const int reductionLimit;
        static tt *transpositionTable;

        static int repetitionIndex;
        static uint64_t repetitionTable[1000];
};







#endif // SEARCH_HPP