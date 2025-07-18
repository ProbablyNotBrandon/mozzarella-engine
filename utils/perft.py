#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
from position import Position
from generate_moves import generate_moves

DEPTH = 6


def perft(pos: Position, depth: int):
    i = 0
    try:
        if depth == 0:
            return 1
        moves = generate_moves(pos)
        if depth == 1:
            return len(moves)
        i = 0
        for move in moves:
            pos.move(move)
            i += perft(pos, depth - 1)
            pos.unmove(move)
    except IndexError:
        print(pos)
        print(depth)
        print(pos.bbs)
        print([bb for bb in pos.bbs])
        print([pos.bbs[x][5] for x in range(2)])
        exit(0)

    return i


if __name__ == "__main__":
    p = Position()
    for i in range(1, 5):
        print(f"Depth: {i}\tNodes: {perft(p, i)}")
""" 
    1	20
2	400
3	8,902
4	197,281
5	4,865,609
              """
