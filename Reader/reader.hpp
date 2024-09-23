#ifndef READER_HPP
#define READER_HPP

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>

namespace Reader {
    class underlying {
    public:
        uint16_t endian_swap_u16(uint16_t x) {
            return (x >> 8) | (x << 8);
        }

        uint32_t endian_swap_u32(uint32_t x) {
            return (x >> 24) | 
                   ((x << 8) & 0x00FF0000) | 
                   ((x >> 8) & 0x0000FF00) | 
                   (x << 24);
        }

        uint64_t endian_swap_u64(uint64_t x) {
            return (x >> 56) | 
                   ((x << 40) & 0x00FF000000000000) | 
                   ((x << 24) & 0x0000FF0000000000) | 
                   ((x << 8)  & 0x000000FF00000000) | 
                   ((x >> 8)  & 0x00000000FF000000) | 
                   ((x >> 24) & 0x0000000000FF0000) | 
                   ((x >> 40) & 0x000000000000FF00) | 
                   (x << 56);
        }
    };

    struct EntryStruct {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    };

    struct BookMove {
        uint8_t toFile = 0;
        uint8_t toRow = 0;
        uint8_t fromFile = 0;
        uint8_t fromRow = 0;
        uint8_t promotion = 0;
    };

    typedef std::vector<BookMove> BookMoves;

    extern std::string Files[8];
    extern std::string Rows[8];

    std::string ConvertBookMoveToUci(BookMove move);
    BookMove RandomBookMove(BookMoves book_moves);

    struct BookEntry {
        BookMove move;
        uint16_t weight;
        uint32_t learn;
    };

    typedef std::vector<BookEntry> BookEntries;

    class Book {
    public:
        void Load(const char *path);
        BookMoves GetBookMoves(uint64_t key, uint16_t minimum_weight = 0);
        BookEntries SearchBook(uint64_t key, uint16_t minimum_weight = 0);
        void Clear();

    private:
        long int num_entries = 0;
        EntryStruct* entries = nullptr;
    };
}

#endif // READER_HPP