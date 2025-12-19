#include "search.h"

TTEntry TT[TT_SIZE];

int _mvv_lva[6][6] = {
    // Attacker: Pawn, Knight, Bishop, Rook, Queen, King
    {105, 205, 305, 405, 505, 605}, // Victim: Pawn
    {104, 204, 304, 404, 504, 604}, // Victim: Knight
    {103, 203, 303, 403, 503, 603}, // Victim: Bishop
    {102, 202, 302, 402, 502, 602}, // Victim: Rook
    {101, 201, 301, 401, 501, 601}, // Victim: Queen
    {100, 200, 300, 400, 500, 600}  // Victim: King
};

int mvv_lva(uint32_t m) {
    if (get_flags(m) & MoveFlags::CAPTURE) return _mvv_lva[get_captured(m)][get_piece(m)];
    return 0;
}

int q_search(Position *p, int ply, int alpha, int beta) {
    int static_eval_white = evaluate(p);
    
    int best_score = (p->player_to_move == Player::WHITE) ? static_eval_white : -static_eval_white;

    if (best_score >= beta) return best_score;
    if (best_score > alpha) alpha = best_score;

    uint32_t noisy_flags = MoveFlags::CAPTURE | MoveFlags::PROMO;

    std::vector<uint32_t> noisy_moves;

    std::vector<uint32_t> moves = generate_legal_moves(p);

    for (uint32_t m: moves) {
        if (get_flags(m) & noisy_flags) noisy_moves.push_back(m);
    }

    std::sort(noisy_moves.begin(), noisy_moves.end(),
              [&](uint32_t x, uint32_t y) {return mvv_lva(x) > mvv_lva(y);});

    for (uint32_t m: noisy_moves) {
        move(p, m);
        int score = -q_search(p, ply + 1, -beta, -alpha);
        unmove(p, m);

        if (score > beta) return score;

        if (score > alpha) alpha = score;
    }

    return alpha;
}

int search(Position *p, int depth, int ply, int alpha, int beta) {
    
    uint64_t z = zobrist(p);
    TTEntry entry = TT[z % TT_SIZE];
    if (entry.key == z && entry.depth >= depth) {
        if (entry.flag == TTEntry::EXACT) return entry.score;
        else if (entry.flag == TTEntry::LOWER && entry.score >= beta) return entry.score;
        else if (entry.flag == TTEntry::UPPER && entry.score <= alpha) return entry.score;
    }

    std::vector<uint32_t> moves = generate_legal_moves(p);

    if (depth == 0) return (p->player_to_move == Player::WHITE) ? evaluate(p) : -evaluate(p);
    // return q_search(p, ply, alpha, beta);

    if (moves.empty()) {
        if (is_in_check(p, p->player_to_move)) return -MATE_SCORE + ply; // Checkmate
        else return 0; // Stalemate
    }

    int alpha_orig = alpha;
    int best_score = INT_MIN;

    std::vector<uint32_t> capture_moves;
    std::vector<uint32_t> non_capture_moves;

    for (uint32_t m: moves) {
        if (get_flags(m) & MoveFlags::CAPTURE) capture_moves.push_back(m);
        else non_capture_moves.push_back(m);
    }

    std::sort(capture_moves.begin(), capture_moves.end(),
              [&](uint32_t x, uint32_t y) {return mvv_lva(x) > mvv_lva(y);});


    for (std::vector<uint32_t> move_set: {capture_moves, non_capture_moves}) {
        for (uint32_t m: move_set) {
            move(p, m);
            int score = -search(p, depth - 1, ply + 1, -beta, -alpha);
            unmove(p, m);

            if (score > alpha) alpha = score;
            
            if (score > best_score) best_score = score;

            if (alpha >= beta) break;
        }
    }

    TTEntry &ins = TT[z % TT_SIZE];
    if (ins.key == 0) TT_OCCUPANCY++;
    ins.key = z;
    ins.score = best_score;
    ins.depth = depth;
    if (best_score <= alpha_orig) ins.flag = TTEntry::UPPER;
    else if (best_score >= beta) ins.flag = TTEntry::LOWER;
    else ins.flag = TTEntry::EXACT;

    return best_score;
}
