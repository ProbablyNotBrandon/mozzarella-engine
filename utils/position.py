#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
import numpy as np
from enum import IntEnum

START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


class Player(IntEnum):
    WHITE = 0
    BLACK = 1


class CastlingRights(IntEnum):
    W_KSIDE = 0b0001
    W_QSIDE = 0b0010
    B_KSIDE = 0b0100
    B_QSIDE = 0b1000


class Position:

    def __init__(self, fen=None):
        # Precondition: fen is a valid FEN string
        fen = fen if fen else START_FEN

        fen = fen.split(" ")

        # Initialize all bitboards to 0
        self.white_pawns = np.uint64(0)
        self.white_knights = np.uint64(0)
        self.white_bishops = np.uint64(0)
        self.white_rooks = np.uint64(0)
        self.white_queens = np.uint64(0)
        self.white_king = np.uint64(0)

        self.black_pawns = np.uint64(0)
        self.black_knights = np.uint64(0)
        self.black_bishops = np.uint64(0)
        self.black_rooks = np.uint64(0)
        self.black_queens = np.uint64(0)
        self.black_king = np.uint64(0)

        # Iterate over board section of the FEN
        j = 8  # Rank number
        i = 56  # Square number
        for rank in fen[0].split("/"):
            for ch in rank:
                match ch:
                    case "P":
                        self.white_pawns |= 1 << i
                        i += 1
                    case "N":
                        self.white_knights |= 1 << i
                        i += 1
                    case "B":
                        self.white_bishops |= 1 << i
                        i += 1
                    case "R":
                        self.white_rooks |= 1 << i
                        i += 1
                    case "Q":
                        self.white_queens |= 1 << i
                        i += 1
                    case "K":
                        self.white_king |= 1 << i
                        i += 1
                    case "p":
                        self.black_pawns |= 1 << i
                        i += 1
                    case "n":
                        self.black_knights |= 1 << i
                        i += 1
                    case "b":
                        self.black_bishops |= 1 << i
                        i += 1
                    case "r":
                        self.black_rooks |= 1 << i
                        i += 1
                    case "q":
                        self.black_queens |= 1 << i
                        i += 1
                    case "k":
                        self.black_king |= 1 << i
                        i += 1
                    # Otherwise we have encountered a digit
                    case _ if ch.isdigit():
                        i += int(ch)
            j -= 1
            i = (j - 1) * 8

        self.player_to_move = Player.WHITE if fen[1] == 'w' else Player.BLACK

        # Castling rights are a 4 bit number
        self.castling_rights = 0
        if 'K' in fen[2]:
            self.castling_rights |= CastlingRights.W_KSIDE
        if 'Q' in fen[2]:
            self.castling_rights |= CastlingRights.W_QSIDE
        if 'k' in fen[2]:
            self.castling_rights |= CastlingRights.B_KSIDE
        if 'q' in fen[2]:
            self.castling_rights |= CastlingRights.B_QSIDE

        if fen[3] == '-':
            self.ep_target = None
        else:
            file = ord(fen[3][0]) - ord('a')
            rank = int(fen[3][1]) - 1
            self.ep_target = rank * 8 + file

        self.halfmoves = int(fen[4])
        self.fullmoves = int(fen[5])


if __name__ == "__main__":
    p = Position()
