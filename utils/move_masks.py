#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
import numpy as np
from position import Player


PAWN_ADVANCE_MASKS = [[np.uint64(0) for _ in range(64)], [np.uint64(0) for _ in range(64)]]
PAWN_ATTACK_MASKS = [[np.uint64(0) for _ in range(64)], [np.uint64(0) for _ in range(64)]]
KNIGHT_MOVE_MASKS = [np.uint64(0) for _ in range(64)]
KING_MOVE_MASKS = [np.uint64(0) for _ in range(64)]


def init_all():
    init_knight_move_masks()
    init_king_move_masks()
    init_pawn_masks()
    np.save("pawn_advance_masks.npy", PAWN_ADVANCE_MASKS)
    np.save("pawn_attack_masks.npy", PAWN_ATTACK_MASKS)
    np.save("knight_move_masks.npy", KNIGHT_MOVE_MASKS)
    np.save("king_move_masks.npy", KING_MOVE_MASKS)



def init_knight_move_masks():
    def generate_knight_move_mask(bit):
        file, rank = bit_to_fr(bit)

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
    print("KNIGHTS")
    print(KNIGHT_MOVE_MASKS)


def init_king_move_masks():
    def generate_king_move_mask(bit):
        file, rank = bit_to_fr(bit)
        mask = np.uint64(0)
        deltas = [(0, 1), (1, 1), (1, 0), (1, -1),
                  (0, -1), (-1, -1), (-1, 0), (-1, 1)]
        for df, dr in deltas:
            f = file + df
            r = rank + dr
            if 0 <= f <= 7 and 0 <= r <= 7:
                mask |= (np.uint64(1) << np.uint64(fr_to_bit(f, r)))
        return mask

    for bit in range(64):
        KING_MOVE_MASKS[bit] = generate_king_move_mask(bit)
    print("KING")
    print(KING_MOVE_MASKS)


def init_pawn_masks():
    def generate_white_pawn_advance_move_mask(bit):
        _, rank = bit_to_fr(bit)
        mask = np.uint64(0)

        if 1 <= rank <= 6:
            mask |= (1 << (bit + 8))
            if rank == 1:
                mask |= (1 << (bit + 16))

        return mask

    def generate_black_pawn_advance_move_mask(bit):
        _, rank = bit_to_fr(bit)
        mask = np.uint64(0)

        if 1 <= rank <= 6:
            mask |= (1 << (bit - 8))
            if rank == 6:
                mask |= (1 << (bit - 16))

        return mask

    def generate_white_pawn_attack_move_mask(bit):
        file, _ = bit_to_fr(bit)
        mask = np.uint64(0)

        if file != 7:
            mask |= (1 << (bit + 9))
        if file != 0:
            mask |= (1 << (bit + 7))
        return mask

    def generate_black_pawn_attack_move_mask(bit):
        file, _ = bit_to_fr(bit)
        mask = np.uint64(0)

        if file != 7:
            mask |= (1 << (bit - 7))
        if file != 0:
            mask |= (1 << (bit - 9))
        return mask

    for bit in range(8, 56):
        PAWN_ADVANCE_MASKS[Player.WHITE][bit] = generate_white_pawn_advance_move_mask(bit)
        PAWN_ADVANCE_MASKS[Player.BLACK][bit] = generate_black_pawn_advance_move_mask(bit)
        PAWN_ATTACK_MASKS[Player.WHITE][bit] = generate_white_pawn_attack_move_mask(bit)
        PAWN_ATTACK_MASKS[Player.BLACK][bit] = generate_black_pawn_attack_move_mask(bit)
    
    print("PAWN ADVANCES")
    print(PAWN_ADVANCE_MASKS)
    print("PAWN ATTACKS")
    print(PAWN_ATTACK_MASKS)


def fr_to_bit(file, rank):
    return rank * 8 + file


def bit_to_fr(bit):
    return (bit % 8, bit // 8)


if __name__ == "__main__":
    init_all()
