#include "position.h"
#include "player.h"
#include "piece.h"
#include "move_generation.h"
#include "move.h"

#include <iostream>


#include <string>
#include <unordered_map>
#include <sstream>

std::string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static std::string square_to_coord(int sq) {
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

int perft_new(Position *p, int depth) {
    if (depth == 0) return 1;
    std::vector<uint32_t> moves = generate_legal_moves(p);
    // for (uint32_t m: moves) {
    //     std::cout << move_to_string(m) << "\n";
    // }
    if (depth == 1) return moves.size();

    int total = 0;
    for (uint32_t m: moves) {
        move(p, m);
        int count = perft_new(p, depth - 1);
        total += count;
        unmove(p, m);
    }

    return total;
}


void perft_divide(Position *p, int depth) {
    std::vector<uint32_t> moves = generate_legal_moves(p);
    int total = 0;

    for (uint32_t m : moves) {
        move(p, m);
        int count = perft_new(p, depth - 1);
        unmove(p, m);

        std::cout << move_to_string(m) << ": " << count << "\n";
        total += count;
    }

    std::cout << "Total: " << total << "\n";
}


int main(__attribute((unused)) int argc, __attribute((unused)) char* argv[]) {
    int DEPTH = std::stoi(argv[1]);
    std::string FEN = argv[2];

    Position p = init_position(FEN);
    

    perft_divide(&p, DEPTH);
}