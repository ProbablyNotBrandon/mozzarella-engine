#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
import numpy as np
from move import *
from enum import IntEnum
from chessutils import bit_to_fr, fr_to_bit, bitscan, u64

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


ch_to_piece = {
    "P": (Player.WHITE, Piece.PAWN), "p": (Player.BLACK, Piece.PAWN),
    "N": (Player.WHITE, Piece.KNIGHT), "n": (Player.BLACK, Piece.KNIGHT),
    "B": (Player.WHITE, Piece.BISHOP), "b": (Player.BLACK, Piece.BISHOP),
    "R": (Player.WHITE, Piece.ROOK), "r": (Player.BLACK, Piece.ROOK),
    "Q": (Player.WHITE, Piece.QUEEN), "q": (Player.BLACK, Piece.QUEEN),
    "K": (Player.WHITE, Piece.KING), "k": (Player.BLACK, Piece.KING),
}

piece_to_ch = {
    (Player.WHITE, Piece.PAWN): "P", (Player.BLACK, Piece.PAWN): "p",
    (Player.WHITE, Piece.KNIGHT): "N", (Player.BLACK, Piece.KNIGHT): "n",
    (Player.WHITE, Piece.BISHOP): "B", (Player.BLACK, Piece.BISHOP): "b",
    (Player.WHITE, Piece.ROOK): "R", (Player.BLACK, Piece.ROOK): "r",
    (Player.WHITE, Piece.QUEEN): "Q", (Player.BLACK, Piece.QUEEN): "q",
    (Player.WHITE, Piece.KING): "K", (Player.BLACK, Piece.KING): "k",
}


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
                if ch.isdigit():
                    i += int(ch)
                else:
                    self.bbs[ch_to_piece[ch][0]][ch_to_piece[ch][1]] |= u64(1 << i)
                    i += 1
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

    def move(self, move: np.uint32):
        from_mask = u64(1 << get_from_sq(move))
        to_mask = u64(1 << get_to_sq(move))

        piece_moved = get_piece(move)

        self.bbs[self.player_to_move][piece_moved] &= ~from_mask

        promotion_piece = get_promotion(move)
        if promotion_piece:
            self.bbs[self.player_to_move][promotion_piece] |= to_mask
        else:
            self.bbs[self.player_to_move][piece_moved] |= to_mask

        captured_piece = get_captured(move)
        if captured_piece:
            self.bbs[1 - self.player_to_move][captured_piece] &= ~to_mask

        self.player_to_move = 1 - self.player_to_move

    def unmove(self, move: np.uint32):
        self.player_to_move = 1 - self.player_to_move
        from_mask = u64(1 << get_from_sq(move))
        to_mask = u64(1 << get_to_sq(move))

        piece_moved = get_piece(move)

        promotion_piece = get_promotion(move)
        if promotion_piece:
            self.bbs[self.player_to_move][promotion_piece] &= ~to_mask
        else:
            self.bbs[self.player_to_move][piece_moved] &= ~to_mask
        self.bbs[self.player_to_move][piece_moved] |= from_mask

        piece_captured = get_captured(move)
        if piece_captured:
            self.bbs[1 - self.player_to_move][piece_captured] |= to_mask

    def get_player_occupied(self, opponent: bool = False) -> np.uint64:
        player = self.player_to_move
        if opponent:
            player = 1 - player

        player_occupied = u64(0)
        for bb in self.bbs[player]:
            player_occupied |= bb

        return player_occupied

    def get_all_occupied(self) -> np.uint64:
        return self.get_player_occupied() | self.get_player_occupied(opponent=True)

    def __repr__(self):
        board = ["." for _ in range(64)]

        for player in (Player.WHITE, Player.BLACK):
            for piece in range(6):
                bitboard = self.bbs[player][piece]
                for sq in range(64):
                    if (bitboard >> sq) & 1:
                        piece_char = piece_to_ch[(player, piece)]
                        board[sq] = piece_char

        lines = []
        for rank in reversed(range(8)):
            line = f"{rank + 1}  "
            for file in range(8):
                line += board[rank * 8 + file] + " "
            lines.append(line.strip())
        lines.append("\n   a b c d e f g h")
        return "\n".join(lines)


if __name__ == "__main__":
    p = Position()
