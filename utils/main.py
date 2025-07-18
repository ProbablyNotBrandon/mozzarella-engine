#!/Users/brandon/sideprojects/chess-bot/venv/bin/python

from position import Position, CastlingRights, Player, Piece
from move_masks import *
from generate_moves import *


def main():

    # Generate starting position of the game
    p = Position()
    moves = generate_moves(p)
    print(moves)
    print(f"Num moves: {len(moves)}")


if __name__ == main():
    main()
