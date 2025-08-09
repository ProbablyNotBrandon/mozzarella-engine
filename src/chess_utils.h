#ifndef __CHESS_UTILS_H__
#define __CHESS_UTILS_H__

#include <cstdint>
#include <map>

#include "piece.h"
#include "player.h"

const std::map<char, Piece> _chtopc = {
    {'p', Piece::PAWN},
    {'n', Piece::KNIGHT},
    {'b', Piece::BISHOP},
    {'r', Piece::ROOK},
    {'q', Piece::QUEEN},
    {'k', Piece::KING},
};

// Get the least significant bit index of the 64-bit bitboard <bb>
inline int pop_lsb(uint64_t &bb);

// Return the player corresponding to the character from the FEN
Player chtopl(char c);

// Return the piece corresponding to the character from the FEN
Piece chtopc(char c);

// Splits a string based on delimiter and returns the pieces as a vector of strings (GPT CODE)
std::vector<std::string> split_str(const std:: string& s, char delimiter);

#endif
