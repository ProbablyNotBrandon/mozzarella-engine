#!/Users/brandon/sideprojects/chess-bot/venv/bin/python

from position import Position, CastlingRights, Player, Piece
from move_masks import *


class Move():
    def __init__(self, src=None, dst=None):
        self.src = src
        self.dst = dst


def main():
    # Generate starting position of the game
    moves = []
    p = Position()
    for idx in 
    moves = generate_moves(p)


def generate_moves(pos):
    moves = []
    moves += generate_rook_moves(pos)
    moves += generate_bishop_moves(pos)
    moves += generate_pawn_moves(pos)
    moves += generate_king_moves(pos)
    moves += generate_queen_moves(pos)


def generate_king_moves(pos: Position):
    # Get the bit that the current king to play is on
    king_bit = bitscan(pos.bbs[pos.player_to_move][Piece.KING])[0]

    # Get the pseudolegal move bitboard for the king's current position
    all_king_moves = KING_MOVE_MASKS[king_bit]
    occupied = np.uint64(0)
    for bb in pos.bbs[pos.player_to_move]:
        occupied |= bb
    new_moves = all_king_moves & ~occupied
    return [Move(king_bit, dst) for dst in bitscan(new_moves)]


def generate_pawn_moves(pos):
    pass


def generate_rook_moves(pos):
    rook_bits = bitscan(pos.bbs[pos.player_to_move][Piece.ROOK])
    move_list = []

    occupied = np.uint64(0)
    for bb in pos.bbs[pos.player_to_move]:
        occupied |= bb

    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[1 - pos.player_to_move]:
        opponent_occupied |= bb

    for rook_bit in rook_bits:
        # all_rook_moves = ROOK_MOVE_MASKS[rook_bit]
        # Vertical
        for dr in [-8, 8]:
            for mul in range(1, 8):
                new_bit = rook_bit + mul * dr
                if not (0 <= new_bit <= 63) or (occupied & (1 << new_bit)):
                    break
                else:
                    move_list.append(Move(rook_bit, new_bit))
                    if opponent_occupied & (1 << new_bit):
                        break

        # Horizontal
        for df in [-1, 1]:
            _, rank = bit_to_fr(rook_bit)
            for mul in range(1, 8):
                new_bit = rook_bit + df * mul
                if (not (0 <= new_bit <= 63) or new_bit // 8 != rank
                    or (occupied & (1 << new_bit))):
                    break
                else:
                    move_list.append(Move(rook_bit, new_bit))
                    if opponent_occupied & (1 << new_bit):
                        break

    return move_list


def generate_queen_moves(pos):


def generate_bishop_moves(pos):


def bitscan(bitboard: np.uint64) -> list[int]:
    """
    This function is magic. I don't know how it works.
    """
    result = []
    b = np.uint64(bitboard)
    while b:
        lsb = b & -b  # isolate least significant bit
        index = int(np.log2(lsb))  # get bit index (0..63)
        result.append(index)
        b ^= lsb  # remove that bit
    return result


if __name__ == main():
    main()
