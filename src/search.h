#ifndef __SEARCH_H__
#define __SEARCH_H__

#include <fstream>

#include "evaluation.h"
#include "move_generation.h"
#include "player.h"
#include "position.h"
#include "timer.h"
#include "tt.h"

const int MATE_SCORE = 1000000;


class MovePicker {
    public:

        MovePicker(int tt_mb);

        ~MovePicker();

        uint32_t find_best_move(Position *p, int ms);
        uint32_t find_best_move_fixed_depth(Position *p, int depth);
        TranspositionTable tt;

        int search(Position *p, int depth, int ply, int alpha = -MATE_SCORE, int beta = MATE_SCORE);
        int q_search(Position *p, int ply, int alpha = INT_MIN, int beta = INT_MAX);
};


// Most Valuable Victim : Least Valuable Attacker - move ordering to get higher value moves,
// like pxQ over lower ones, like qxP
int mvv_lva(uint32_t move);

#endif
