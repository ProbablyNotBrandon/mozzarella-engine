#include "search.h"

int _mvv_lva[6][6] = {
    // Attacker: Pawn, Knight, Bishop, Rook, Queen, King
    {105, 205, 305, 405, 505, 605}, // Victim: Pawn
    {104, 204, 304, 404, 504, 604}, // Victim: Knight
    {103, 203, 303, 403, 503, 603}, // Victim: Bishop
    {102, 202, 302, 402, 502, 602}, // Victim: Rook
    {101, 201, 301, 401, 501, 601}, // Victim: Queen
    {100, 200, 300, 400, 500, 600}  // Victim: King
};


/*
 * Most Valuable Victim : Least Valuable Attacker - move ordering to get higher value moves.
 *
 * Example: pxQ should be chosen over qxP.
 */
int mvv_lva(uint32_t m) {
    if (get_flags(m) & MoveFlags::CAPTURE) return _mvv_lva[get_captured(m)][get_piece(m)];
    return 0;
}


/*
 * Constructor for the MovePicker class.
 * tt_mb is the size of the transposition table in MB.
 */
MovePicker::MovePicker(int tt_mb) :
    tt(tt_mb) {}


/*
 * Destructor for the MovePicker class.
 */
MovePicker::~MovePicker() {
    // TODO: implement?
}


/*
 * Public facing interface for the MovePicker. Returns the 32-bit encoded move which represents
 * the optimal move to make in the given position.
 *
 * Internally relies on search() and q_search() in order to search the game space.
 */
uint32_t MovePicker::find_best_move(Position *p, int depth) {
    
    std::ofstream log("log", std::ios::app);

    int best_score = INT_MIN;
    uint32_t best_move;

    std::vector<uint32_t> moves = generate_legal_moves(p);

    for (uint32_t m: moves) {
        p->move(m);
        int score = -(this->search(p, depth - 1, 0, -MATE_SCORE, MATE_SCORE));
        log << "Searching move: " << move_to_string(m) << "\tScore: " << score << std::endl;
        p->unmove(m);

        if (score > best_score) {
            best_score = score;
            best_move = m;
        }
    }

    log << "Making move " << move_to_string(best_move) << " with score " << best_score << std::endl;
    return best_move;
}


/*
 * Alpha-beta minimax search which recursively searches positions and returns the score of the
 * best move according to the evaluation function at the given depth.
 */
int MovePicker::search(Position *p, int depth, int ply, int alpha, int beta) {
    
    // Probe the transposition table first
    uint64_t z = p->zobrist();
    int probe_score;
    if (this->tt.probe(z, depth, &probe_score, alpha, beta)) return probe_score;

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
            p->move(m);
            int score = -(this->search(p, depth - 1, ply + 1, -beta, -alpha));
            p->unmove(m);

            if (score > alpha) alpha = score;
            
            if (score > best_score) best_score = score;

            if (alpha >= beta) break;
        }
    }

    // Store new entry in the transposition table
    this->tt.store(z, depth, best_score, alpha_orig, beta);

    return best_score;
}


/*
 * Quiescence search which only returns evaluations of "quiet" positions, otherwise, continually
 * searches "noisy" positions until they become quiet.
 *
 * A "quiet" position is one where there is no immediate tactical move,
 * such as a capture/check/promotion.
 */
int MovePicker::q_search(Position *p, int ply, int alpha, int beta) {
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
        p->move(m);
        int score = -q_search(p, ply + 1, -beta, -alpha);
        p->unmove(m);

        if (score > beta) return score;

        if (score > alpha) alpha = score;
    }

    return alpha;
}
