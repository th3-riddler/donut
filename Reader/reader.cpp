#include "reader.hpp"

namespace Reader {
    std::string Files[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    std::string Rows[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};

    std::string ConvertBookMoveToUci(BookMove move) {
        std::string move_string = "";
        move_string += Files[move.fromFile];
        move_string += Rows[move.fromRow];
        move_string += Files[move.toFile];
        move_string += Rows[move.toRow];
        if (move.promotion != 0) {
            move_string += "nbrq"[move.promotion - 1];
        }
        return move_string;
    }

    BookMove RandomBookMove(BookMoves book_moves) {
        if (book_moves.empty()) {
            throw std::runtime_error("No book moves available");
        }
        std::vector<BookMove> move;
        size_t nelems = 1;
        std::sample(
            book_moves.begin(),
            book_moves.end(),
            std::back_inserter(move),
            nelems,
            std::mt19937{std::random_device{}()}
        );
        return move[0];
    }

    void Book::Load(const char *path) {
        FILE *file = std::fopen(path, "rb");

        if (file == NULL) {
            std::cerr << "<Error> Please use valid book" << std::endl;
            return;
        } else {
            std::fseek(file, 0, SEEK_END);
            long position = std::ftell(file);

            if (position < sizeof(EntryStruct)) {
                std::cerr << "<Error> No entries found" << std::endl;
                return;
            }

            num_entries = position / sizeof(EntryStruct);

            entries = (EntryStruct*)std::malloc(num_entries * sizeof(EntryStruct));
            std::rewind(file);

            size_t returnValue = std::fread(entries, sizeof(EntryStruct), num_entries, file);

            std::fclose(file);
        }
    }

    BookMoves Book::GetBookMoves(uint64_t key, uint16_t minimum_weight) {
        EntryStruct *entry;
        uint16_t move;
        BookMove book_move;
        underlying u;
        BookMoves bookMoves;
        for (entry = entries; entry < entries + num_entries; entry++) {
            if (u.endian_swap_u64(entry->key) == key && u.endian_swap_u16(entry->weight) >= minimum_weight) {
                move = u.endian_swap_u16(entry->move);
                book_move.fromFile = ((move >> 6) & 7);
                book_move.fromRow = ((move >> 9) & 7);    
                book_move.toFile = ((move >> 0) & 7);
                book_move.toRow = ((move >> 3) & 7);  
                book_move.promotion = ((move >> 12) & 7); 
                bookMoves.push_back(book_move);
            }
        }
        return bookMoves;
    }

    BookEntries Book::SearchBook(uint64_t key, uint16_t minimum_weight) {
        EntryStruct *entry;
        uint16_t move;
        BookMove book_move;
        underlying u;
        BookEntry book_entry;
        BookEntries bookEntries;
        for (entry = entries; entry < entries + num_entries; entry++) {
            if (u.endian_swap_u64(entry->key) == key && u.endian_swap_u16(entry->weight) >= minimum_weight) {
                move = u.endian_swap_u16(entry->move);
                book_move.fromFile = ((move >> 6) & 7);
                book_move.fromRow = ((move >> 9) & 7);    
                book_move.toFile = ((move >> 0) & 7);
                book_move.toRow = ((move >> 3) & 7);  
                book_move.promotion = ((move >> 12) & 7); 
                book_entry.move = book_move;
                book_entry.weight = u.endian_swap_u16(entry->weight);
                book_entry.learn = u.endian_swap_u32(entry->learn);
                bookEntries.push_back(book_entry);
            }
        }
        return bookEntries;
    }

    void Book::Clear() {
        std::free(entries);
        entries = nullptr;
        num_entries = 0;
    }
}