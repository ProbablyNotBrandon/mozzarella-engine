#!/Users/brandon/sideprojects/chess-bot/venv/bin/python

from enum import IntEnum
import numpy as np

from chess_utils import u64, bit_to_fr
from move import *

START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


class Player(IntEnum):
    WHITE = 0
    BLACK = 1


class Piece(IntEnum):
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING = range(6)


class CastlingRights(IntEnum):
    WHITE_KING  = 0b000001
    WHITE_QROOK = 0b000010
    WHITE_KROOK = 0b000100
    BLACK_KING  = 0b001000
    BLACK_QROOK = 0b010000
    BLACK_KROOK = 0b100000


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
    # The player to move
    player_to_move: Player

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
            self.castling_rights |= CastlingRights.WHITE_KING | CastlingRights.WHITE_KROOK
        if 'Q' in fen[2]:
            self.castling_rights |= CastlingRights.WHITE_KING | CastlingRights.WHITE_QROOK
        if 'k' in fen[2]:
            self.castling_rights |= CastlingRights.BLACK_KING | CastlingRights.BLACK_KROOK
        if 'q' in fen[2]:
            self.castling_rights |= CastlingRights.BLACK_KING | CastlingRights.BLACK_QROOK

        if fen[3] == '-':
            self.ep_target = None
        else:
            file = ord(fen[3][0]) - ord('a')
            rank = int(fen[3][1]) - 1
            self.ep_target = rank * 8 + file

        self.halfmoves = int(fen[4])
        self.fullmoves = int(fen[5])

    def move(self, move: np.uint32):
        # CASTLING LOGIC
        flags_mask = get_flags(move)
        if flags_mask == KING_CASTLE: # will we have permission to do this?
            self.bbs[self.player_to_move][Piece.KING] >>= np.uint64(2)
            self.bbs[self.player_to_move][Piece.ROOK] &= ~(self.bbs[self.player_to_move][Piece.KING] >> u64(1))
            self.bbs[self.player_to_move][Piece.ROOK] |= self.bbs[self.player_to_move][Piece.KING] << u64(1)
            self.castling_rights |= u64(0b011 << (self.player_to_move * 2))  # Remove player's right to castle
            return
        elif flags_mask == QUEEN_CASTLE:
            self.bbs[self.player_to_move][Piece.KING] <<= np.uint64(3)
            self.bbs[self.player_to_move][Piece.ROOK] &= ~(self.bbs[self.player_to_move][Piece.KING] << u64(1))
            self.bbs[self.player_to_move][Piece.ROOK] |= self.bbs[self.player_to_move][Piece.KING] >> u64(1)
            self.castling_rights |= u64(0b101 << (self.player_to_move * 2))
            return

        from_mask = u64(1 << get_from_sq(move))
        to_mask = u64(1 << get_to_sq(move))

        piece_moved = get_piece(move)

        self.bbs[self.player_to_move][piece_moved] &= ~from_mask

        promotion_piece = get_promotion(move)
        if promotion_piece:
            self.bbs[self.player_to_move][promotion_piece] |= to_mask
        else:
            self.bbs[self.player_to_move][piece_moved] |= to_mask

        
        if get_flags(move) == CAPTURE:
            captured_piece = get_captured(move)
            if not to_mask:
                print("NO TO MASK")
            if not self.bbs[1 - self.player_to_move][captured_piece] & to_mask:
                wrong = -1
                for piece in range(6):
                    if self.bbs[1 - self.player_to_move][piece] & to_mask:
                        wrong = piece
                print(f"INCORRECT PIECE IN CAPTURE: {get_captured(move)} {wrong}")
            self.bbs[1 - self.player_to_move][captured_piece] &= ~to_mask

        self.player_to_move = Player(1 - self.player_to_move)

    def unmove(self, move: np.uint32):
        self.player_to_move = Player(1 - self.player_to_move)

        flags_mask = get_flags(move)
        if flags_mask == KING_CASTLE: # will we have permission to do this?
            self.bbs[self.player_to_move][Piece.ROOK] |= self.bbs[self.player_to_move][Piece.KING] >> u64(1)
            self.bbs[self.player_to_move][Piece.ROOK] &= ~(self.bbs[self.player_to_move][Piece.KING] << u64(1))
            self.bbs[self.player_to_move][Piece.KING] <<= np.uint64(2)
            self.castling_rights &= ~u64(0b011 << (self.player_to_move * 2))  # Add back player's right to castle
            return
        elif flags_mask == QUEEN_CASTLE:
            self.bbs[self.player_to_move][Piece.ROOK] &= ~(self.bbs[self.player_to_move][Piece.KING] >> u64(1))
            self.bbs[self.player_to_move][Piece.ROOK] |= self.bbs[self.player_to_move][Piece.KING] << u64(1)
            self.bbs[self.player_to_move][Piece.KING] >>= np.uint64(3)
            self.castling_rights &= ~u64(0b101 << (self.player_to_move * 2))
            return

        from_mask = u64(1 << get_from_sq(move))
        to_mask = u64(1 << get_to_sq(move))

        piece_moved = get_piece(move)

        promotion_piece = get_promotion(move)
        if promotion_piece:
            self.bbs[self.player_to_move][promotion_piece] &= ~to_mask
        else:
            self.bbs[self.player_to_move][piece_moved] &= ~to_mask
        self.bbs[self.player_to_move][piece_moved] |= from_mask

        if get_flags(move) == CAPTURE:
            piece_captured = get_captured(move)
            self.bbs[1 - self.player_to_move][piece_captured] |= to_mask

    def get_player_occupied(self, player=None) -> np.uint64:
        if player is None:
            player = self.player_to_move

        player_occupied = u64(0)
        for bb in self.bbs[player]:
            player_occupied |= bb

        return player_occupied

    def get_all_occupied(self) -> np.uint64:
        return self.get_player_occupied(Player.WHITE) | self.get_player_occupied(Player.BLACK)

    def __repr__(self):
        assert_unique_occupancy(self)
        board = ["." for _ in range(64)]

        for player in (Player.WHITE, Player.BLACK):
            for piece in range(6):
                bitboard = self.bbs[player][piece]
                for sq in range(64):
                    if (bitboard >> sq) & 1:
                        piece_char = piece_to_ch[(player, Piece(piece))]
                        board[sq] = piece_char

        lines = []
        for rank in reversed(range(8)):
            line = f"{rank + 1}  "
            for file in range(8):
                line += board[rank * 8 + file] + " "
            lines.append(line.strip())
        lines.append("\n   a b c d e f g h")
        return "\n".join(lines)


### GPT CODE
def assert_unique_occupancy(p):
    occupancy_map = [""] * 64
    piece_names = ["P", "N", "B", "R", "Q", "K"]

    for player in (0, 1):
        for piece_type in range(6):
            bitboard = p.bbs[player][piece_type]
            sq = 0
            while bitboard:
                if bitboard & 1:
                    if occupancy_map[sq]:
                        print(f"Overlap on square {sq} ({square_name(sq)})!")
                        print(f"  Existing: {occupancy_map[sq]}")
                        print(f"  Overlap:  {('WHITE' if player == 0 else 'BLACK')} {piece_names[piece_type]}")
                    else:
                        occupancy_map[sq] = f"{'W' if player == 0 else 'B'}{piece_names[piece_type]}"
                bitboard >>= 1
                sq += 1

def square_name(index):
    file = index % 8
    rank = index // 8
    return f"{chr(ord('a') + file)}{rank + 1}"
###

if __name__ == "__main__":
    p = Position()
