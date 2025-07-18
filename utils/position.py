#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
import numpy as np
from move import Move
from enum import IntEnum

START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


class Player(IntEnum):
    WHITE = 0
    BLACK = 1


class Piece(IntEnum):
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING = range(6)


class CastlingRights(IntEnum):
    W_KSIDE = 0b0001
    W_QSIDE = 0b0010
    B_KSIDE = 0b0100
    B_QSIDE = 0b1000


class Position:

    def __init__(self, fen=None):
        self.bbs = [[np.uint64(0)] * 6, [np.uint64(0)] * 6]
        # Precondition: fen is a valid FEN string

        fen = fen if fen else START_FEN

        fen = fen.split(" ")
        # Iterate over board section of the FEN
        j = 8  # Rank number
        i = 56  # Square number
        for rank in fen[0].split("/"):
            for ch in rank:
                match ch:
                    case "P":
                        self.bbs[Player.WHITE][Piece.PAWN] |= np.uint64(1 << i)
                        i += 1
                    case "N":
                        self.bbs[Player.WHITE][Piece.KNIGHT] |= np.uint64(1 << i)
                        i += 1
                    case "B":
                        self.bbs[Player.WHITE][Piece.BISHOP] |= np.uint64(1 << i)
                        i += 1
                    case "R":
                        self.bbs[Player.WHITE][Piece.ROOK] |= np.uint64(1 << i)
                        i += 1
                    case "Q":
                        self.bbs[Player.WHITE][Piece.QUEEN] |= np.uint64(1 << i)
                        i += 1
                    case "K":
                        self.bbs[Player.WHITE][Piece.KING] |= np.uint64(1 << i)
                        i += 1
                    case "p":
                        self.bbs[Player.BLACK][Piece.PAWN] |= np.uint64(1 << i)
                        i += 1
                    case "n":
                        self.bbs[Player.BLACK][Piece.KNIGHT] |= np.uint64(1 << i)
                        i += 1
                    case "b":
                        self.bbs[Player.BLACK][Piece.BISHOP] |= np.uint64(1 << i)
                        i += 1
                    case "r":
                        self.bbs[Player.BLACK][Piece.ROOK] |= np.uint64(1 << i)
                        i += 1
                    case "q":
                        self.bbs[Player.BLACK][Piece.QUEEN] |= np.uint64(1 << i)
                        i += 1
                    case "k":
                        self.bbs[Player.BLACK][Piece.KING] |= np.uint64(1 << i)
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

    def move(self, move: Move):
        from_mask = np.uint64(1 << move.from_sq)
        to_mask = np.uint64(1 << move.to_sq)

        self.bbs[self.player_to_move][move.piece_moved] &= ~from_mask

        if move.promotion_piece:
            self.bbs[self.player_to_move][move.promotion_piece] |= to_mask
        else:
            self.bbs[self.player_to_move][move.piece_moved] |= to_mask

        if move.piece_captured:
            self.bbs[1 - self.player_to_move][move.piece_captured] &= ~to_mask

        self.player_to_move = 1 - self.player_to_move

    def unmove(self, move: Move):
        from_mask = np.uint64(1 << move.from_sq)
        to_mask = np.uint64(1 << move.to_sq)

        if move.promotion_piece:
            self.bbs[self.player_to_move][move.promotion_piece] &= ~to_mask
        else:
            self.bbs[self.player_to_move][move.piece_moved] &= ~to_mask
        self.bbs[self.player_to_move][move.piece_moved] |= from_mask

        if move.piece_captured:
            self.bbs[1 - self.player_to_move][move.piece_captured] |= to_mask
        self.player_to_move = 1 - self.player_to_move


if __name__ == "__main__":
    p = Position()
