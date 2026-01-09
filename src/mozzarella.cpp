#include "utils.h"
#include "search.h"
#include "uci.h"


void uci_loop() {
    std::string line;
    Position pos;
    MovePicker mp = MovePicker(64);

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name Mozzarella" << std::endl;
            std::cout << "id author Brandon Thiessen" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (line == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (line == "position startpos") {
            pos = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }
        else if (line.rfind("position", 0) == 0) {
            size_t moves_idx = line.find("moves");
            if (moves_idx != std::string::npos) {
                std::string moves_str = line.substr(moves_idx + 6);
                std::istringstream iss(moves_str);
                std::string move_str;
                while (iss >> move_str);
                uint32_t m = parse_move(pos, move_str);
                pos.move(m);
            }
        }
        else if (line.rfind("go", 0) == 0) {

            int wtime = -1, btime = -1;
            int winc = 0, binc = 0; // currently unused
            int movetime = -1;
            int depth = -1;

            std::istringstream iss(line);
            std::string token;
            iss >> token; // read "go"

            // Read in all values from "go" command
            while (iss >> token) {
                if (token == "wtime") iss >> wtime;
                else if (token == "btime") iss >> btime;
                else if (token == "winc") iss >> winc;
                else if (token == "binc") iss >> binc;
                else if (token == "movetime") iss >> movetime;
                else if (token == "depth") iss >> depth;
            }

            int remaining = (pos.player_to_move == Player::WHITE) ? wtime : btime;
            int time_budget = (movetime != -1) ? movetime : (remaining > 0 ? remaining / 50 : 1000);

            uint32_t best_move;

            // Search either to a fixed depth depending on "go" request
            if (depth != -1) {
                best_move = mp.find_best_move_fixed_depth(&pos, depth);
            } else {
                best_move = mp.find_best_move(&pos, time_budget);
            }

            // Make the move on the internal board
            pos.move(best_move);

            // Output the move to CLI
            std::cout << "bestmove " << move_to_string(best_move) << std::endl;

        }
        else if (line == "quit") {
            break;
        }
    }
}


int main() {
    std::ios::sync_with_stdio(false);
    uci_loop();
    return 0;
}
