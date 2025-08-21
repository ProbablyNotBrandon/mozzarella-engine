#ifndef __POSITION_H__
#define __POSITION_H__

#include <string>
#include <iostream>
#include <map>
#include <chrono>
#include <random>

#include "castling_rights.h"
#include "move.h"
#include "piece.h"
#include "player.h"
#include "chess_utils.h"



// Position struct, containing all relevant game state information
struct Position {
    uint64_t bitboards[2][6]; // indexed with [player][piece]
    Player player_to_move;
    uint8_t castling_rights[2];
    int halfmoves; // currently unused
    int fullmoves; // currently unused
    int ep_target; // currently unused
    int material_value[2];
    std::vector<uint32_t> move_stack;
    std::vector<uint8_t> castling_rights_stack; // Push/pop 2 items at a time
};

// Initialize and return a Position struct from the given FEN string
Position init_position(std::string fen);

// Decode the encoded move and make the move, changing p's state
void move(Position *p, uint32_t move);

// Decode the encoded move and unmake the move, changing p's state
void unmove(Position *p, uint32_t move);

// Get a bitboard of all the squares occupied by the chosen player
uint64_t get_occupied(Position *p, int player);

// Initialize the Zobrist hashing array (64 random ULL numbers)
void init_zobrist();

// Returns the (64 bit) Zobrist hash of the position
uint64_t zobrist(Position *p);

#endif
