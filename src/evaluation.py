from position import Position, Player
from move_generation import *
import numpy as np


def minimax(p: Position, depth: int) -> tuple[int, np.uint32 | None]:
    """
    Returns the minimax score
    """
    in_check = is_in_check(p, p.player_to_move)
    moves = generate_legal_moves(p)

    # Node is a terminating node
    if depth == 0:
        return evaluate(p, p.player_to_move), None
    if len(moves) == 0:
        if in_check:
            return -9999999, None
        else:
            return 0, None
    best_score = -9999999
    best_move = None
    for move in moves:

        p.move(move)
        score, _ = minimax(p, depth - 1)
        score = -score
        if score > best_score:
            best_score = score
            best_move = move
        p.unmove(move)

    return best_score, best_move


def evaluate(p: Position, player: Player):
    """
    Calculates the delta of total piece value of each player.
    """
    opponent = 1 - player
    player_sum = 0
    opponent_sum = 0

    player_sum += 100 * len(bitscan(p.bbs[player][Piece.PAWN]))
    player_sum += 300 * len(bitscan(p.bbs[player][Piece.KNIGHT]))
    player_sum += 300 * len(bitscan(p.bbs[player][Piece.BISHOP]))
    player_sum += 500 * len(bitscan(p.bbs[player][Piece.ROOK]))
    player_sum += 900 * len(bitscan(p.bbs[player][Piece.QUEEN]))

    opponent_sum += 100 * len(bitscan(p.bbs[opponent][Piece.PAWN]))
    opponent_sum += 300 * len(bitscan(p.bbs[opponent][Piece.KNIGHT]))
    opponent_sum += 300 * len(bitscan(p.bbs[opponent][Piece.BISHOP]))
    opponent_sum += 500 * len(bitscan(p.bbs[opponent][Piece.ROOK]))
    opponent_sum += 900 * len(bitscan(p.bbs[opponent][Piece.QUEEN]))

    return player_sum - opponent_sum
