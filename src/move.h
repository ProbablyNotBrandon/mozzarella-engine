#ifndef __MOVE_H__
#define __MOVE_H__

#include <cstdint>

#include "piece.h"

// Bit width of each field:
// from_sq:     bits 0-5
// to_sq:       bits 6-11
// piece:       bits 12-15
// captured:    bits 16-19
// promotion:   bits 20-22
// flags:       bits 23-25
extern uint32_t _FROM_SQ_MASK;
extern uint32_t _TO_SQ_MASK;
extern uint32_t _PIECE_MASK;
extern uint32_t _CAPTURED_MASK;
extern uint32_t _PROMOTION_MASK;
extern uint32_t _FLAGS_MASK;

// Flag encoding
enum MoveFlags : uint32_t {
    DOUBLE_PAWN_PUSH    = 1 << 21,
    EN_PASSANT          = 1 << 22,
    KING_CASTLE         = 1 << 23,
    QUEEN_CASTLE        = 1 << 24,
    CASTLE              = (1 << 23 | 1 << 24),
    CAPTURE             = 1 << 25,
    KNIGHT_PROMO        = 1 << 26,
    BISHOP_PROMO        = 1 << 27,
    ROOK_PROMO          = 1 << 28,
    QUEEN_PROMO         = 1 << 29,
    PROMO               = (1 << 26 | 1 << 27 | 1 << 28 | 1 << 29)
};

// Encode the move into a compact representation
uint32_t encode_move(int from_sq, int to_sq, int piece, int captured=0, int promotion=0, int flags=0);

// Function declarations for move decoding
int get_from_sq(uint32_t move);
int get_to_sq(uint32_t move);
Piece get_piece(uint32_t move);
Piece get_captured(uint32_t move);
Piece get_promotion(uint32_t move);
uint32_t get_flags(uint32_t move);

#endif