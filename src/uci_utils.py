import numpy as np
from position import *
from move import *
from move_generation import generate_legal_moves


def move_to_uci(move: np.uint32) -> str:
    from_sq = get_from_sq(move)
    to_sq = get_to_sq(move)
    promotion = get_promotion(move)

    uci = square_to_coord(from_sq) + square_to_coord(to_sq)
    if promotion:
        promo_char = "nbrq"[promotion - 1]  # assuming 1=knight, 2=bishop, etc.
        uci += promo_char
    return uci


def uci_to_move(p: Position, uci: str) -> Move:
    def coord_to_square(coord: str) -> int:
        file = ord(coord[0]) - ord('a')
        rank = int(coord[1]) - 1
        return rank * 8 + file

    from_sq = coord_to_square(uci[:2])
    to_sq = coord_to_square(uci[2:4])
    promotion = 0

    if len(uci) == 5:
        promo_char = uci[4].lower()
        promo_map = {'n': 1, 'b': 2, 'r': 3, 'q': 4}
        promotion = promo_map[promo_char]

    # Find legal matching move
    for move in generate_legal_moves(p, print_out=False):
        if get_from_sq(move) == from_sq and get_to_sq(move) == to_sq:
            if get_promotion(move) == promotion:
                return move

    raise ValueError("Illegal or unrecognized move.")
