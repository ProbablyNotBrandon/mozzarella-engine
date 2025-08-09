#include "move.h"

uint32_t encode_move(int from_sq, int to_sq, int piece, int captured=-1, int promotion=0, int flags=0) {
    if (captured >= 0) { // TODO: modified this solution, review (if captured is a pawn, flag capture will resolve to false)
        flags |= CAPTURE;
    }
    return (uint32_t)(
        from_sq |
        to_sq << 6 |
        piece << 12 |
        captured << 15 |
        promotion << 18 |
        flags << 21);
}


int get_from_sq(uint32_t move) {
    return (int)(move & _FROM_SQ_MASK);
}

int get_to_sq(uint32_t move) {
    return (int)((move & _TO_SQ_MASK) >> 6);
}

Piece get_piece(uint32_t move) {
    return (Piece)((move & _PIECE_MASK) >> 12);
}

Piece get_captured(uint32_t move) {
    return (Piece)((move & _CAPTURED_MASK) >> 15);
}

Piece get_promotion(uint32_t move) {
    return (Piece)((move & _PROMOTION_MASK) >> 18);
}

uint32_t get_flags(uint32_t move) {
    return move & _FLAGS_MASK;
}
