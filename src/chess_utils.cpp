#include "chess_utils.h"

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

Player chtopl(char c) {
    if (c == 'p' || c == 'n' || c == 'b' || c == 'r' || c == 'q' || c == 'k') {
        return Player::WHITE;
    } else if (c == 'P' || c == 'N' || c == 'B' || c == 'R' || c == 'Q' || c == 'K'){
        return Player::BLACK;
    } else {
        // Piece is not valid
        return (Player) -1;
    }
}

Piece chtopc(char c) {
    return _chtopc.at(std::tolower(c));
}


// GPT CODE
// Splits a string based on delimiter and returns the pieces as a vector of strings
std::vector<std::string> split_str(const std:: string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;

    for (char c: s) {
        if (c == delimiter) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}
// END GPT CODE
