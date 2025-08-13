#include "move_pick.h"

uint32_t find_best_move(Position *p, int depth) {

    int best_score = INT_MIN;
    uint32_t best_move;

    std::vector<uint32_t> moves = generate_legal_moves(p);

    for (uint32_t m: moves) {
        move(p, m);
        int score = -search(p, depth - 1);
        unmove(p, m);

        if (score > best_score) {
            best_score = score;
            best_move = m;
        }
    }

    return best_move;
}
