#!/Users/brandon/sideprojects/chess-bot/venv/bin/python

from position import Position, CastlingRights, Player, Piece
from move_masks import *


class Move():
    def __init__(self, src=None, dst=None):
        self.src = src
        self.dst = dst


def main():
    # Generate starting position of the game
    p = Position()
    moves = generate_moves(p)
    print(moves)


def generate_moves(pos):
    moves = []
    moves += generate_pawn_moves(pos)
    moves += generate_knight_moves(pos)
    moves += generate_bishop_moves(pos)
    moves += generate_rook_moves(pos)
    moves += generate_queen_moves(pos)
    moves += generate_king_moves(pos)
    return moves


def generate_pawn_moves(pos):
    player = pos.player_to_move
    opponent = 1 - player
    pawn_bits = bitscan(pos.bbs[player][Piece.PAWN])

    occupied = np.uint64(0)
    for bb in pos.bbs[player]:
        occupied |= bb

    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[opponent]:
        occupied |= bb

    all_occupied = occupied | opponent_occupied

    move_list = []

    for pawn_bit in pawn_bits:
        all_pawn_advances = (
                PAWN_ADVANCE_MASKS[player][pawn_bit] & ~all_occupied)

        # Edge case: a pawn cannot advance two steps if obstructed
        if player == Player.WHITE and 8 <= pawn_bit <= 15:
            if all_occupied & (1 << (pawn_bit + 8)):
                all_pawn_advances &= ~(1 << pawn_bit + 16)
        elif player == Player.BLACK and 48 <= pawn_bit <= 55:
            if all_occupied & (1 << (pawn_bit - 8)):
                all_pawn_advances &= ~(1 << pawn_bit - 16)

        for dst_bit in bitscan(all_pawn_advances):
            move_list.append(Move(pawn_bit, dst_bit))

        # A pawn can only capture a square if the enemy is there
        all_pawn_attacks = (
                PAWN_ATTACK_MASKS[player][pawn_bit] & opponent_occupied)

        for dst_bit in bitscan(all_pawn_attacks):
            move_list.append(Move(pawn_bit, dst_bit))

    return move_list


def generate_knight_moves(pos):
    player = pos.player_to_move
    opponent = 1 - player

    occupied = np.uint64(0)
    for bb in pos.bbs[player]:
        occupied |= bb

    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[opponent]:
        opponent_occupied |= bb

    move_list = []
    knight_bits = bitscan(pos.bbs[player][Piece.KNIGHT])

    for knight_bit in knight_bits:
        all_knight_moves = KNIGHT_MOVE_MASKS
        all_knight_moves |= occupied

        dsts = bitscan(all_knight_moves)

        for dst in dsts:
            move_list.append(Move(knight_bit, dst))

    return move_list


def generate_bishop_moves(pos):
    return generate_sliding_moves(pos, Piece.BISHOP, [-9, -7, 7, 9])


def generate_rook_moves(pos):
    return generate_sliding_moves(pos, Piece.ROOK, [-8, -1, 1, 8])


def generate_queen_moves(pos):
    return generate_sliding_moves(pos, Piece.QUEEN,
                                  [-9, -8, -7, -1, 1, 7, 8, 9])


def generate_king_moves(pos: Position):
    player = pos.player_to_move

    # Get the bit that the current king to play is on
    king_bit = bitscan(pos.bbs[player][Piece.KING])[0]

    # Get the pseudolegal move bitboard for the king's current position
    all_king_moves = KING_MOVE_MASKS[king_bit]
    occupied = np.uint64(0)
    for bb in pos.bbs[pos.player_to_move]:
        occupied |= bb
    new_moves = all_king_moves & ~occupied
    return [Move(king_bit, dst) for dst in bitscan(new_moves)]


def generate_sliding_moves(pos: Position, piece: Piece, deltas: list[int]):
    player = pos.player_to_move
    opponent = 1 - player

    occupied = np.uint64(0)
    for bb in pos.bbs[player]:
        occupied |= bb

    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[opponent]:
        occupied |= bb

    move_list = []
    piece_bits = bitscan(pos.bbs[player][piece])

    for piece_bit in piece_bits:
        current_bit = piece_bit
        for d in deltas:
            for _ in range(1, 8):
                dst_bit = current_bit + d

                if not (0 <= dst_bit <= 63) or (occupied & (1 << dst_bit)):
                    break
                if (d in [-9, -7, -1, 1, 7, 9]
                    and abs((current_bit % 8) - (dst_bit % 8)) > 1):
                    break
                else:
                    move_list.append(Move(piece_bit, dst_bit))
                    if (opponent_occupied & (1 << dst_bit)):
                        break

                current_bit = dst_bit

    return move_list


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
