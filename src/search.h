#ifndef __SEARCH_H__
#define __SEARCH_H__

#include "evaluation.h"
#include "move_generation.h"
#include "player.h"
#include "position.h"

// Search function, returns the score of the best move according to the evaluation function
// at the given depth.
int search(Position *p, int depth, int alpha = INT_MIN, int beta = INT_MAX);

#endif
