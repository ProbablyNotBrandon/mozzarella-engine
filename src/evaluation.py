from position import Position, Player
from move_generation import *
import numpy as np


def minimax(p: Position, depth: int, alpha: int = -9999999, beta: int = 9999999) -> tuple[int, np.uint32 | None]:
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
        score, _ = minimax(p, depth - 1, -beta, -alpha)
        score = -score
        p.unmove(move)

        if score > best_score:
            best_score = score
            best_move = move

        alpha = max(alpha, score)
        if alpha >= beta:
            break

    return best_score, best_move


def evaluate(p: Position, player: Player):
    """
    Calculates the delta of total piece value of each player.
    """
    return p.total_material_value[player] - p.total_material_value[1 - player]
