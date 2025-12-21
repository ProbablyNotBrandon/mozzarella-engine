#include <chrono>
#include <iostream>
#include "../src/position.h"
#include "../src/search.h"


int main() {
    int n = 4;
    int m = 20;

    std::cout << "Generating " << m << " searches of depth " << n << std::endl;

    std::string POS_STRING = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

    auto start = std::chrono::steady_clock::now();

    Position p = Position(POS_STRING);
    MovePicker mp = MovePicker(64);

    for (int i = 0; i < m; i++) {
        auto best = mp.find_best_move_fixed_depth(&p, n);
        p.move(best);
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

    std::cout << "Done\n";

    std::cout << "Time elapsed: " << elapsed << "ms\n";

    return 0;
}
