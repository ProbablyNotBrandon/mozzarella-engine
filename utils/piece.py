from enum import Enum


class Piece(Enum):
    W_PAWN = 0b00000001
    W_KNIGHT = 0b00000010
    W_BISHOP = 0b00000100
    W_ROOK = 0b00001000
    W_QUEEN = 0b00010000
    W_KING = 0b00100000

    B_PAWN = 0b01000001
    B_KNIGHT = 0b01000010
    B_BISHOP = 0b01000100
    B_ROOK = 0b01001000
    B_QUEEN = 0b01010000
    B_KING = 0b01100000


def is_white(bits):
    if bits & 0b01000000:
        return True
    else:
        return False

