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
    int pst_eval;
    int material_value[2];
    std::vector<uint32_t> move_stack;
    std::vector<uint8_t> castling_rights_stack; // Push/pop 2 items at a time
    std::vector<int> pst_eval_stack; // Store recent position eval
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


// Piece Square Tables
// [player][piece][square]
const int PST[2][5][64] = {
    {   // White
        // Pawns
        {
             0,  0,  0,  0,  0,  0,  0,  0,
             5, 10, 10,-20,-20, 10, 10,  5,
             5, -5,-10,  0,  0,-10, -5,  5,
             0,  0,  0, 20, 20,  0,  0,  0,
             5,  5, 10, 25, 25, 10,  5,  5,
            10, 10, 20, 30, 30, 20, 10, 10,
            50, 50, 50, 50, 50, 50, 50, 50,
             0,  0,  0,  0,  0,  0,  0,  0
        },
        // Knights
        {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
        },
        // Bishops
        {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -20,-10,-10,-10,-10,-10,-10,-20
        },
        // Rooks
        {
              0,  0,  0,  5,  5,  0,  0,  0,
             -5,  0,  0,  0,  0,  0,  0, -5,
             -5,  0,  0,  0,  0,  0,  0, -5,
             -5,  0,  0,  0,  0,  0,  0, -5,
             -5,  0,  0,  0,  0,  0,  0, -5,
             -5,  0,  0,  0,  0,  0,  0, -5,
              5, 10, 10, 10, 10, 10, 10,  5,
              0,  0,  0,  0,  0,  0,  0,  0
        },
        // Queens
        {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -10,  5,  5,  5,  5,  5,  0,-10,
              0,  0,  5,  5,  5,  5,  0, -5,
             -5,  0,  5,  5,  5,  5,  0, -5,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        }
    },
    {   // Black (negated + flipped vertically)
        // Pawns
        {
             0,  0,  0,  0,  0,  0,  0,  0,
           -50,-50,-50,-50,-50,-50,-50,-50,
           -10,-10,-20,-30,-30,-20,-10,-10,
            -5, -5,-10,-25,-25,-10, -5, -5,
             0,  0,  0,-20,-20,  0,  0,  0,
            -5,  5, 10,  0,  0, 10,  5, -5,
            -5,-10,-10, 20, 20,-10,-10, -5,
             0,  0,  0,  0,  0,  0,  0,  0
        },
        // Knights
        {
             50, 40, 30, 30, 30, 30, 40, 50,
             40, 20,  0,  0,  0,  0, 20, 40,
             30,  0,-10,-15,-15,-10,  0, 30,
             30, -5,-15,-20,-20,-15, -5, 30,
             30,  0,-15,-20,-20,-15,  0, 30,
             30, -5,-10,-15,-15,-10, -5, 30,
             40, 20,  0, -5, -5,  0, 20, 40,
             50, 40, 30, 30, 30, 30, 40, 50
        },
        // Bishops
        {
             20, 10, 10, 10, 10, 10, 10, 20,
             10,  0,  0,  0,  0,  0,  0, 10,
             10,  0, -5,-10,-10, -5,  0, 10,
             10,  0,-10,-10,-10,-10,  0, 10,
             10,-10,-10,-10,-10,-10,-10, 10,
             10, -5,-10,-10,-10,-10, -5, 10,
             10, -5,  0,  0,  0,  0, -5, 10,
             20, 10, 10, 10, 10, 10, 10, 20
        },
        // Rooks
        {
              0,  0,  0,  0,  0,  0,  0,  0,
             -5,-10,-10,-10,-10,-10,-10, -5,
              5,  0,  0,  0,  0,  0,  0,  5,
              5,  0,  0,  0,  0,  0,  0,  5,
              5,  0,  0,  0,  0,  0,  0,  5,
              5,  0,  0,  0,  0,  0,  0,  5,
              0,  0,  0, -5, -5,  0,  0,  0,
              0,  0,  0,  0,  0,  0,  0,  0
        },
        // Queens
        {
             20, 10, 10,  5,  5, 10, 10, 20,
             10,  0,  0,  0,  0,  0,  0, 10,
             10,  0, -5, -5, -5, -5,  0, 10,
              0,  0, -5, -5, -5, -5,  0,  5,
              5,  0, -5, -5, -5, -5,  0,  5,
             10,  0, -5, -5, -5, -5,  0, 10,
             10,  0,  0,  0,  0,  0,  0, 10,
             20, 10, 10,  5,  5, 10, 10, 20
        }
    }
};

#endif
