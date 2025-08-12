#ifndef __EVALUATION_H__
#define __EVALUATION_H__

#include "player.h"
#include "position.h"

// Evaluation function that returns the evaluation of the current board position
int evaluate(Position *p, Player player);

#endif
