#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
import sys
from position import Position
from move import *
from move_generation import generate_legal_moves
from datetime import datetime
from time import sleep

DEPTH = 5
if len(sys.argv) == 2:
    DEPTH = int(sys.argv[1])


def perft(pos: Position, depth: int):
    if depth == 0:
        return 1
    moves = generate_legal_moves(pos, print_out=False)
    if depth == 1:
        return len(moves)
    total = 0
    for move in moves:
        pos.move(move)
        count = perft(pos, depth - 1)
        total += count
        pos.unmove(move)
    return total

def perft_divide(pos, depth):
    moves = generate_legal_moves(pos, print_out=False)
    total = 0
    for move in moves:
        pos.move(move)
        count = perft(pos, depth - 1)
        print(f"{square_to_coord(get_from_sq(move))}{square_to_coord(get_to_sq(move))}: {count}")
        total += count
        pos.unmove(move)
    print(f"Total: {total}")
    return total


if __name__ == "__main__":
    exp_perft = [1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860]
    p = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
    start = datetime.now()
    for i in range(DEPTH + 1):
        print(f"Depth: {i}\tNodes: {perft(p, i)}\tExpected: {exp_perft[i]}")
    end = datetime.now()
    print(f"Elapsed time: {end - start}")
