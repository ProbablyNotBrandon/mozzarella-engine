#ifndef __MOVE_GENERATION_H__
#define __MOVE_GENERATION_H__



#include <vector>
#include <iostream>
#include <cmath>
#include <cstdint>

#include "position.h"
#include "piece.h"
#include "player.h"


extern uint64_t PAWN_ADVANCE_MASKS[2][64];
extern uint64_t PAWN_ATTACK_MASKS[2][64];
extern uint64_t KNIGHT_MOVE_MASKS[64];
extern uint64_t KING_MOVE_MASKS[64];

// Masks to check and see if the squares that need to be crossed in
// order to castle are blocked
extern uint64_t W_QS_CASTLE_MASK;
extern uint64_t W_KS_CASTLE_MASK;
extern uint64_t B_QS_CASTLE_MASK;
extern uint64_t B_KS_CASTLE_MASK;

std::vector<uint32_t> generate_legal_moves(Position *p);

std::vector<uint32_t> generate_castle_moves(Position *p);
std::vector<uint32_t> generate_en_passant_moves(Position *p);

std::vector<uint32_t> generate_pawn_moves(Position *p);
std::vector<uint32_t> generate_knight_moves(Position *p);

std::vector<uint32_t> generate_king_moves(Position *p);

std::vector<uint32_t> generate_bishop_moves(Position *p);
std::vector<uint32_t> generate_rook_moves(Position *p);
std::vector<uint32_t> generate_queen_moves(Position *p);

std::vector<uint32_t> generate_sliding_moves(Position *p, Piece piece, int deltas[], int ndeltas);

bool is_in_check(Position *p, Player player);
bool is_in_sliding_check(Position *p, Player player);

void render_bitboard(uint64_t bb, int origin_sq, char symbol);

#endif