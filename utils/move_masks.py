#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
import numpy as np

KNIGHT_MOVE_MASKS = [np.uint64(0) for _ in range(64)]
BISHOP_MOVE_MASKS = [np.uint64(0) for _ in range(64)]
ROOK_MOVE_MASKS = [np.uint64(0) for _ in range(64)]
QUEEN_MOVE_MASKS = [np.uint64(0) for _ in range(64)]
KING_MOVE_MASKS = [np.uint64(0) for _ in range(64)]


def init_all():
    init_knight_move_masks()
    init_bishop_move_masks()
    init_rook_move_masks()
    QUEEN_MOVE_MASKS = [
        BISHOP_MOVE_MASKS[i] & ROOK_MOVE_MASKS[i]
        for i in range(64)
    ]


def init_knight_move_masks():
    def generate_knight_move_mask(bit):
        file = bit % 8
        rank = bit // 8

        deltas = [
            (1, 2), (-1, 2), (1, -2), (-1, -2),
            (2, 1), (-2, 1), (2, -1), (-2, -1)
        ]

        mask = np.uint64(0)
        for df, dr in deltas:
            f = file + df
            r = rank + dr
            if 0 <= f <= 7 and 0 <= r <= 7:
                mask |= (np.uint64(1) << np.uint64(r * 8 + f))
        return mask

    for bit in range(64):
        KNIGHT_MOVE_MASKS[bit] = generate_knight_move_mask(bit)
    print(KNIGHT_MOVE_MASKS)


def fr_to_bit(file, rank):
    return rank * 8 + file


def bit_to_fr(bit):
    return (bit % 8, bit // 8)


def init_bishop_move_masks():
    pass


def init_rook_move_masks():
    pass


def init_pawn_move_masks():
    pass


if __name__ == "__main__":
    init_all()
