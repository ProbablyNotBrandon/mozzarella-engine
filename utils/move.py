from chessutils import *


class Move():
    def __init__(self, from_sq, to_sq, piece_moved, piece_captured=None, promotion_piece=None):
        self.from_sq = from_sq
        self.to_sq = to_sq
        self.piece_moved = piece_moved
        self.piece_captured = piece_captured
        self.promotion_piece = promotion_piece

    def __repr__(self):
        src_file, src_rank = bit_to_fr(self.from_sq)
        dst_file, dst_rank = bit_to_fr(self.to_sq)
        src_rank += 1
        dst_rank += 1
        return f"Move<{chr(src_file + ord('a'))}{src_rank} to {chr(dst_file + ord('a'))}{dst_rank}>"
