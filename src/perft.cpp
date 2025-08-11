#include "position.h"
#include "move_generation.h"

#include <iostream>


#include <string>



std::string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


long perft(Position *p, int depth) {
    if (depth == 0) return 1;
    std::vector<uint32_t> moves = generate_legal_moves(p);
    if (depth == 1) return moves.size();

    long total = 0;
    for (uint32_t m: moves) {
        move(p, m);
        long count = perft(p, depth - 1);
        total += count;
        unmove(p, m);
    }

    return total;
}



int main(int argc, char* argv[]) {
    if (argc != 2) return -1;
    std::cout << "Running standard perft\n";
    int DEPTH = std::stoi(argv[1]);
    // std::string FEN = argv[2];

    // Position p = init_position(FEN);
    Position p = init_position(STARTPOS);

    long exp_perft[] = {1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860};

    for (int i = 0; i <= DEPTH; i++) {
        std::cout << "Depth: " << i << "\tNodes: " << perft(&p, i) << "\t\tExpected: " << exp_perft[i] << "\n";
    }

    std::cout << "\nPosition 2\n";

    std::string pos2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Position p2 = init_position(pos2);

    long pos2_exp[] = {1, 48, 2039, 97862, 4085603, 193690690, 8031647685};

    for (int i = 0; i <= 5; i++) {
        std::cout << "Depth: " << i << "\tNodes: " << perft(&p2, i) << "\t\tExpected: " << pos2_exp[i] << "\n";
    }

    std::cout << "\nPosition 3\n";
    std::string pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    Position p3 = init_position(pos3);
    long pos3_exp[] = {1, 14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};

    for (int i = 0; i <= 6; i++) {
        std::cout << "Depth: " << i << "\tNodes: " << perft(&p3, i) << "\t\tExpected: " << pos3_exp[i] << "\n";
    }

    std::cout << "\nPosition 4\n";
    std::string pos4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    Position p4 = init_position(pos4);
    long pos4_exp[] = {1, 6, 264, 9467, 422333, 15833292, 706045033};

    for (int i = 0; i <= 6; i++) {
        std::cout << "Depth: " << i << "\tNodes: " << perft(&p4, i) << "\t\tExpected: " << pos4_exp[i] << "\n";
    }
}

