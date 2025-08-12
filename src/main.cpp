#include "position.h"
#include "move_pick.h"
#include "chess_utils.h"

int main(__attribute((unused)) int argc, __attribute((unused)) char * argv[]) {

    Position p = init_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    render_board(&p);

    while (true) {
        std::cout << "White thinking...\n";

        uint32_t best = find_best_move(&p, 5);

        if (!best) {break;}

        std::cout << "White plays: " << move_to_string(best) << "\n";
        move(&p, best);

        render_board(&p);

        std::cout << "Black thinking...\n";

        best = find_best_move(&p, 5);

        if (!best) {break;}

        std::cout << "Black plays: " << move_to_string(best) << "\n";
        move(&p, best);
        render_board(&p);
    }

    if (is_in_check(&p, p.player_to_move)) std::cout << "CHECKMATE\n";
    else std::cout << "STALEMATE\n";



}
