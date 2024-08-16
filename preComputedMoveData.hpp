#ifndef PRECOMPUTEDMOVEDATA_HPP
#define PRECOMPUTEDMOVEDATA_HPP



class preComputedMoveData {
    public:
        static const int directionOffsets[];
        static int numSquaresToEdge[8][8];

        static void initialize();
        static void cleanupNumSquaresToEdge();
};














#endif // PRECOMPUTEDMOVEDATA_HPP