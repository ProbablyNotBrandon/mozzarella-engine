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

f = open("moves.log", "w")


def perft(pos: Position, depth: int):
    i = 0
    try:
        if depth == 0:
            return 1
        moves = generate_legal_moves(pos)
        # print([Move(move) for move in moves])
        if depth == 1:
            return len(moves)
        i = 0
        for move in moves:
            pos.move(move)
            i += perft(pos, depth - 1)
            pos.unmove(move)
    except IndexError:
        pass

    return i


if __name__ == "__main__":
    exp_perft = [1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860]
    p = Position()
    start = datetime.now()
    for i in range(DEPTH + 1):
        print(f"Depth: {i}\tNodes: {perft(p, i)}\tExpected: {exp_perft[i]}")
    end = datetime.now()
    print(f"Elapsed time: {end - start}")
