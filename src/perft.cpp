#include "position.h"
#include "move_generation.h"

#include <iostream>


#include <string>



std::string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


long perft(Position *p, int depth) {
    if (depth == 0) return 1;
    std::vector<uint32_t> moves = generate_legal_moves(p);
    if (depth == 1) return moves.size();

    long total = 0;
    for (uint32_t m: moves) {
        move(p, m);
        long count = perft(p, depth - 1);
        total += count;
        unmove(p, m);
    }

    return total;
}



int main(__attribute((unused)) int argc, __attribute((unused)) char* argv[]) {
    int DEPTH = std::stoi(argv[1]);
    // std::string FEN = argv[2];

    // Position p = init_position(FEN);
    Position p = init_position(STARTPOS);

    long exp_perft[] = {1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860};

    for (int i = 0; i <= DEPTH; i++) {
        std::cout << "Depth: " << i << "\tNodes: " << perft(&p, i) << "\t\tExpected: " << exp_perft[i] << "\n";
    }
}

