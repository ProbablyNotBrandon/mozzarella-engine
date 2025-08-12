#ifndef __CHESS_UTILS_H__
#define __CHESS_UTILS_H__

#include <cstdint>
#include <map>
#include <sstream>
#include <iostream>
#include <string>

#include "piece.h"
#include "player.h"
#include "move.h"

const std::map<char, Piece> _chtopc = {
    {'p', Piece::PAWN},
    {'n', Piece::KNIGHT},
    {'b', Piece::BISHOP},
    {'r', Piece::ROOK},
    {'q', Piece::QUEEN},
    {'k', Piece::KING},
};

// Get the least significant bit index of the 64-bit bitboard <bb>
#if defined(__GNUC__) || defined(__clang__)
inline int pop_lsb(uint64_t &bb) {
    int index = __builtin_ctzll(bb);  // find LSB index
    bb &= bb - 1;                     // clear LSB
    return index;
}
#elif defined(_MSC_VER)
#include <intrin.h>
inline int pop_lsb(uint64_t &bb) {
    unsigned long index;
    _BitScanForward64(&index, bb);
    bb &= bb - 1;
    return static_cast<int>(index);
}
#else
inline int pop_lsb(uint64_t &bb) {
    int index = 0;
    uint64_t b = bb;
    while ((b & 1) == 0) {
        b >>= 1;
        ++index;
    }
    bb &= bb - 1;
    return index;
}
#endif

// Translate a square number to a standard coordinate string.
std::string square_to_coord(int sq);

// Return the string form of an encoded move.
std::string move_to_string(uint32_t move);

// Return the player corresponding to the character from the FEN
Player chtopl(char c);

// Return the piece corresponding to the character from the FEN
Piece chtopc(char c);

// Splits a string based on delimiter and returns the pieces as a vector of strings (GPT CODE)
std::vector<std::string> split_str(const std:: string& s, char delimiter);


// Output the chess board to the console.
void render_board(void *v);
#endif
