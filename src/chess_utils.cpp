#include "chess_utils.h"
#include "position.h"

const char piece_chars[2][6] = {
    { 'P', 'N', 'B', 'R', 'Q', 'K' }, // White
    { 'p', 'n', 'b', 'r', 'q', 'k' }  // Black
};

void render_board(void *v) {
    Position *p = (Position *)v;
    
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " ";
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            char piece_char = '.';
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

std::string square_to_coord(int sq) {
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return std::string{file, rank};
}

std::string move_to_string(uint32_t move) {
    int from_sq = get_from_sq(move);
    int to_sq   = get_to_sq(move);
    // uint32_t flags = get_flags(move);

    // Map single flags to their names
    // static const std::unordered_map<uint32_t, std::string> flag_names = {
    //     {DOUBLE_PAWN_PUSH,        "Double Pawn Push"},
    //     {EN_PASSANT,              "En Passant"},
    //     {KING_CASTLE,             "King Castle"},
    //     {QUEEN_CASTLE,            "Queen Castle"},
    //     {CAPTURE,                 "Capture"},
    //     {KNIGHT_PROMO,            "Knight Promo"},
    //     {BISHOP_PROMO,            "Bishop Promo"},
    //     {ROOK_PROMO,              "Rook Promo"},
    //     {QUEEN_PROMO,             "Queen Promo"},
    // };

    std::ostringstream oss;
    oss << square_to_coord(from_sq) << square_to_coord(to_sq); //s" (";

    // bool first = true;
    // for (auto &kv : flag_names) {
    //     if (flags & kv.first) {
    //         if (!first) oss << " ";
    //         oss << kv.second;
    //         first = false;
    //     }
    // }

    // if (first) { // No flags matched
    //     oss << "Quiet";
    // }

    // oss << ")";
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
