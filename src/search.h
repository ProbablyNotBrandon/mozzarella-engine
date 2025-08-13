#ifndef __SEARCH_H__
#define __SEARCH_H__

#include "evaluation.h"
#include "move_generation.h"
#include "player.h"
#include "position.h"

// Most Valuable Victim : Least Valuable Attacker - move ordering to get higher value moves,
// like pxQ over lower ones, like qxP
int mvv_lva(uint32_t move);
// Search function, returns the score of the best move according to the evaluation function
// at the given depth.
int search(Position *p, int depth, int alpha = INT_MIN, int beta = INT_MAX);

// Quiescence search, only returns evaluations of "quiet" positions
// (ones where there is no immediate tactical move, such as a capture/check/promotion)
int q_search(Position *p, int alpha = INT_MIN, int beta = INT_MAX);

#endif
