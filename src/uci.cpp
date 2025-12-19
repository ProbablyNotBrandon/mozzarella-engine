#include "uci_utils.h"

void uci_loop() {
    std::string line;
    Position pos;

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
            // Example: "go depth 5"
            int depth = 4;
            size_t depth_idx = line.find("depth");
            if (depth_idx != std::string::npos) {
                depth = std::stoi(line.substr(depth_idx + 6));
            }

            uint32_t best = find_best_move(&pos, depth);
            pos.move(best);
            std::cout << "bestmove " << move_to_string(best) << std::endl;
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
