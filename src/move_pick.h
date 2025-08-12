#ifndef __MOVE_PICK_H__
#define __MOVE_PICK_H__

#include <cstdint>

#include "move_generation.h"
#include "position.h"
#include "search.h"

// Returns the move with the best score according to the
// search function.
uint32_t find_best_move(Position *p, int depth);

#endif
