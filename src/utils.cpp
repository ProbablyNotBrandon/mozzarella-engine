#include "utils.h"

const std::string piece_chars[2][6] = {
    { "♟", "♞", "♝", "♜", "♛", "♚" }, // White
    { "♙", "♘", "♗", "♖", "♕", "♔" }  // Black
};

void render_board(void *v) {
    Position *p = (Position *)v;
    
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " ";
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            std::string piece_char = ".";
            for (int pl = 0; pl < 2; ++pl) {
                for (int pc = 0; pc < 6; ++pc) {
                    if (p->bitboards[pl][pc] & (1ULL << sq)) {
                        piece_char = piece_chars[pl][pc];
                        goto found;
                    }
                }
            }
            found:
            std::cout << piece_char << " ";
        }
        std::cout << "\n";
    }
    std::cout << "  a b c d e f g h\n";
}


int coord_to_sq(std::string s) {
    int file = s[0] - 'a';         // 'a'..'h' → 0..7
    int rank = s[1] - '1';         // '1'..'8' → 0..7
    return rank * 8 + file;        // row-major, a1=0, h8=63
}


std::string square_to_coord(int sq) {
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return std::string{file, rank};
}

std::string move_to_string(uint32_t move) {
    int from_sq = get_from_sq(move);
    int to_sq   = get_to_sq(move);
    std::ostringstream oss;
    oss << square_to_coord(from_sq) << square_to_coord(to_sq);
    uint32_t flags = get_flags(move);
    if (flags & KNIGHT_PROMO) {
        oss << "k";
    } else if (flags & BISHOP_PROMO) {
        oss << "b";
    } else if (flags & ROOK_PROMO) {
        oss << "r";
    } else if (flags & QUEEN_PROMO) {
        oss << "q";
    }
    return oss.str();
}

Player chtopl(char c) {
    if (c == 'P' || c == 'N' || c == 'B' || c == 'R' || c == 'Q' || c == 'K'){
        return Player::WHITE;
    } else if (c == 'p' || c == 'n' || c == 'b' || c == 'r' || c == 'q' || c == 'k') {
        return Player::BLACK;
    }
    else {
        // Piece is not valid
        return (Player) -1;
    }
}

Piece chtopc(char c) {
    return _chtopc.at(std::tolower(c));
}


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


