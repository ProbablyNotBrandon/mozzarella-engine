#ifndef __UCI_UTILS_H__
#define __UCI_UTILS_H__

#include <iostream>
#include <string>
#include "position.h"
#include "search.h"
#include "chess_utils.h"

// Parse a UCI move into the engine's encoding
uint32_t parse_move(Position pos, std::string move_str);

#endif
