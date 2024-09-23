#include <cstdint>
#ifndef POLYKEYS_HPP
#define POLYKEYS_HPP

struct polyBookEntry {
    uint64_t key;
    uint16_t move;
    uint16_t weight;
    uint32_t learn;
};

class Polyglot {
    public:
        static const uint64_t Random64Poly[781];
        long numEntries;
        polyBookEntry *entries;
};

#endif