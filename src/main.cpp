#include "position.h"
#include "move_pick.h"
#include "chess_utils.h"
#include "move_generation.h"

#include <random>
#include <chrono>

static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

int main(__attribute((unused)) int argc, __attribute((unused)) char * argv[]) {

    Position p = init_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    render_board(&p);

    while (true) {
        std::cout << "White thinking...\n";

        uint32_t best = find_best_move(&p, 40);

        if (!best) {break;}

        std::cout << "White plays: " << move_to_string(best) << "\n";
        move(&p, best);

        render_board(&p);

        std::cout << "Black thinking...\n";

        // best = find_best_move(&p, 5);

        // if (!best) {break;}

        // std::cout << "Black plays: " << move_to_string(best) << "\n";
        // move(&p, best);

        std::vector<uint32_t> black_moves = generate_legal_moves(&p);
        if (black_moves.empty()) break;

        std::uniform_int_distribution<std::size_t> dist(0, black_moves.size() - 1);
        size_t random_index = dist(rng);

        uint32_t black_move = black_moves[random_index];

        std::cout << "Black plays: " << move_to_string(black_move) << "\n";
        move(&p, black_move);

        render_board(&p);
    }

    if (is_in_check(&p, p.player_to_move)) std::cout << "CHECKMATE\n";
    else std::cout << "STALEMATE\n";



}
