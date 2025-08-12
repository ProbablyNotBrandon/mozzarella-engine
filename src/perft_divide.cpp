#include "position.h"
#include "player.h"
#include "move_generation.h"
#include "move.h"

#include <iostream>


#include "chess_utils.h"

std::string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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
