#!/Users/brandon/sideprojects/chess-bot/venv/bin/python

from position import Position, CastlingRights, Player
from precomputed_moves import *

class Move():
    def __init__(src=None, dst=None):
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

def generate_king_moves(pos):

def generate_pawn_moves(pos):

def generate_rook_moves(pos):

def generate_queen_moves(pos):

def generate_bishop_moves(pos):
    

if __name__ == main():
    main()
