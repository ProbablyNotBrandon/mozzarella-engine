#include "move_pick.h"

uint32_t find_best_move(Position *p, int depth) {
    
    std::ofstream log("log", std::ios::app);

    int best_score = INT_MIN;
    uint32_t best_move;

    std::vector<uint32_t> moves = generate_legal_moves(p);

    for (uint32_t m: moves) {
        move(p, m);
        int score = -search(p, depth - 1, 0, -MATE_SCORE, MATE_SCORE);
        log << "Searching move: " << move_to_string(m) << "\tScore: " << score << std::endl;
        unmove(p, m);

        if (score > best_score) {
            best_score = score;
            best_move = m;
        }
    }

    log << "Making move " << move_to_string(best_move) << " with score " << best_score << std::endl;
    return best_move;
}
