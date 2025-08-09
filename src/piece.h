#ifndef __PIECE_H__
#define __PIECE_H__

enum Piece: int { PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5 };

// Declaration of function that returns the material value of a piece
int piece_value(Piece piece);

#endif