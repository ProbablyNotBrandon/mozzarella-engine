#include <chrono>
#include <iostream>
#include "../src/position.h"
#include "../src/move_generation.h"


int main() {
    int n = 200;
    int m = 10000;

    std::cout << "Generating " << m << " sets of " << n << " random walks\n";

    std::string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < m; i++) {

        int seed = 789617 + i;
        std::mt19937 rng(seed);

        Position p = Position(STARTPOS);

        for (int j = 0; j < n; j++) {

            auto moves = generate_legal_moves(&p);
            if (moves.empty()) break;

            std::uniform_int_distribution<uint64_t> dist(0, moves.size() - 1);
            uint32_t move = moves[dist(rng)];

            p.move(move);

        }
    }


    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

    std::cout << "Done\n";

    std::cout << "Time elapsed: " << elapsed << "ms\n";

    return 0;
}
