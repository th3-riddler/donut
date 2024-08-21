#ifndef EVALUATION_HPP
#define EVALUATION_HPP

class Evaluation {
    public:
        static const int materialScore[12];
        static const int pawnScore[64];
        static const int knightScore[64];
        static const int bishopScore[64];
        static const int rookScore[64];
        static const int queenScore[64];
        static const int kingScore[64];
        static const int mirrorPieceScore[128];
        static const int mvvLva[12][12];

        static int evaluate();
        static int scoreMove(int move);
};




#endif // EVALUATION_HPP