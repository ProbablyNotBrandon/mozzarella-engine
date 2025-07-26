import numpy as np

# Bit width of each field:
# from_sq:     bits 0-5
# to_sq:       bits 6-11
# piece:       bits 12-15
# captured:    bits 16-19
# promotion:   bits 20-22
# flags:       bits 23-25

_FROM_SQ_MASK   = 0b00000000000000000000000000111111  # 6 bits
_TO_SQ_MASK     = 0b00000000000000000000111111000000  # next 6 bits
_PIECE_MASK     = 0b00000000000000000111000000000000  # next 4 bits
_CAPTURED_MASK  = 0b00000000000000111000000000000000  # next 4 bits
_PROMOTION_MASK = 0b00000000000111000000000000000000  # next 3 bits
_FLAGS_MASK     = 0b00000001111000000000000000000000  # next 3 bits

# Move flags (4 bits)
QUIET_MOVE              = 0b0000
DOUBLE_PAWN_PUSH        = 0b0001
KING_CASTLE             = 0b0010
QUEEN_CASTLE            = 0b0011
CAPTURE                 = 0b0100
EN_PASSANT              = 0b0101

# Promotions (no capture)
KNIGHT_PROMO            = 0b1000
BISHOP_PROMO            = 0b1001
ROOK_PROMO              = 0b1010
QUEEN_PROMO             = 0b1011

# Promotions with capture
KNIGHT_PROMO_CAPTURE    = 0b1100
BISHOP_PROMO_CAPTURE    = 0b1101
ROOK_PROMO_CAPTURE      = 0b1110
QUEEN_PROMO_CAPTURE     = 0b1111


def encode_move(from_sq, to_sq, piece, captured=0, promotion=0, flags=0) -> np.uint32:
    if captured:
        flags |= CAPTURE
    return np.uint32(from_sq |
                     (to_sq << 6) |
                     (piece << 12) |
                     (captured << 15) |
                     (promotion << 18) |
                     (flags << 21))


def get_from_sq(move: np.uint32) -> int:
    return int(move & _FROM_SQ_MASK)


def get_to_sq(move: np.uint32) -> int:
    return int((move & _TO_SQ_MASK) >> 6)


def get_piece(move: np.uint32) -> int:
    return int((move & _PIECE_MASK) >> 12)


def get_captured(move: np.uint32) -> int:
    return int((move & _CAPTURED_MASK) >> 15)


def get_promotion(move: np.uint32) -> int:
    return int((move & _PROMOTION_MASK) >> 18)


def get_flags(move: np.uint32) -> int:
    return int((move & _FLAGS_MASK) >> 21)


class Move:
    def __init__(self, encoded_move: np.uint32):
        self.encoded = encoded_move

    def __repr__(self) -> str:
        return move_to_string(self.encoded)


def move_to_string(move: np.uint32) -> str:
    from_sq = get_from_sq(move)
    to_sq = get_to_sq(move)
    flags = get_flags(move)

    flag_names = {
        QUIET_MOVE: "Quiet",
        DOUBLE_PAWN_PUSH: "Double Pawn Push",
        KING_CASTLE: "King Castle",
        QUEEN_CASTLE: "Queen Castle",
        CAPTURE: "Capture",
        EN_PASSANT: "En Passant",
        KNIGHT_PROMO: "Knight Promo",
        BISHOP_PROMO: "Bishop Promo",
        ROOK_PROMO: "Rook Promo",
        QUEEN_PROMO: "Queen Promo",
        KNIGHT_PROMO_CAPTURE: "Knight Promo Capture",
        BISHOP_PROMO_CAPTURE: "Bishop Promo Capture",
        ROOK_PROMO_CAPTURE: "Rook Promo Capture",
        QUEEN_PROMO_CAPTURE: "Queen Promo Capture",
    }

    def square_to_coord(sq):
        file = chr((sq % 8) + ord('a'))
        rank = str((sq // 8) + 1)
        return file + rank

    return f"[{flag_names.get(flags, f'Unknown ({flags})')}] {square_to_coord(from_sq)} → {square_to_coord(to_sq)}"

