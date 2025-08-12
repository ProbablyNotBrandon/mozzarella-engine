#include "search.h"


int search(Position *p, int depth, int alpha, int beta) {

    std::vector<uint32_t> moves = generate_legal_moves(p);

    if (depth == 0) return evaluate(p, p->player_to_move);

    if (moves.empty()) {
        if (is_in_check(p, p->player_to_move)) return INT_MIN; // Checkmate
        else return 0; // Stalemate
    }

    int best_score = INT_MIN;

    for (uint32_t m: moves) {
        move(p, m);
        int score = -search(p, depth - 1, -beta, -alpha);
        unmove(p, m);

        if (score > alpha) alpha = score;
        
        if (score > best_score) best_score = score;

        if (alpha >= beta) break;
    }

    return best_score;
}
