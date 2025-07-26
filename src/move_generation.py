#!/Users/brandon/sideprojects/chess-bot/venv/bin/python
import numpy as np
from chess_utils import *
from move import *
from move_masks import *
from position import Position, CastlingRights, Player, Piece


PAWN_ADVANCE_MASKS = np.load("move_masks/pawn_advance_masks.npy")
PAWN_ATTACK_MASKS = np.load("move_masks/pawn_attack_masks.npy")
KNIGHT_MOVE_MASKS = np.load("move_masks/knight_move_masks.npy")
KING_MOVE_MASKS = np.load("move_masks/king_move_masks.npy")

NK_ATTACK_MASKS = {
    Piece.KNIGHT: KNIGHT_MOVE_MASKS,
    Piece.KING: KING_MOVE_MASKS
}


def generate_moves(pos: Position):
    moves = []
    moves += generate_pawn_moves(pos)
    moves += generate_knight_moves(pos)
    moves += generate_bishop_moves(pos)
    moves += generate_rook_moves(pos)
    moves += generate_queen_moves(pos)
    moves += generate_king_moves(pos)
    moves += generate_castle_moves(pos)

    return moves

def generate_legal_moves(p: Position):
    player = p.player_to_move
    moves = generate_moves(p)
    legal_moves = []
    for move in moves:
        p.move(move)
        if not is_in_check(p, Player(player)):
            legal_moves.append(move)
        p.unmove(move)
    return legal_moves


def generate_castle_moves(pos: Position):
    w_qs_castle_mask = np.uint64((1 << 1) | (1 << 2) | (1 << 3))
    w_ks_castle_mask = np.uint64((1 << 5) | (1 << 6))

    b_qs_castle_mask = np.uint64((1 << 57) | (1 << 58) | (1 << 59))
    b_ks_castle_mask = np.uint64((1 << 62) | (1 << 61))

    all_occupied = pos.get_all_occupied()

    moves = []

    if pos.player_to_move == Player.WHITE:
        if not (pos.castling_rights & (CastlingRights.WHITE_KING | CastlingRights.WHITE_QROOK)) and not (w_qs_castle_mask & all_occupied):
            moves.append(encode_move(4, 2, Piece.KING, flags=QUEEN_CASTLE))
        if not (pos.castling_rights & (CastlingRights.WHITE_KING | CastlingRights.WHITE_KROOK)) and not (w_ks_castle_mask & all_occupied):
            moves.append(encode_move(4, 6, Piece.KING, flags=KING_CASTLE))
    elif pos.player_to_move == Player.BLACK:
        if not (pos.castling_rights & (CastlingRights.BLACK_KING | CastlingRights.BLACK_QROOK)) and not (b_qs_castle_mask & all_occupied):
            moves.append(encode_move(60, 58, Piece.KING, flags=QUEEN_CASTLE))
        if not (pos.castling_rights & (CastlingRights.BLACK_KING | CastlingRights.BLACK_KROOK)) and not (b_ks_castle_mask & all_occupied):
            moves.append(encode_move(60, 62, Piece.KING, flags=KING_CASTLE))

    return moves


def generate_pawn_moves(pos):
    player = pos.player_to_move
    opponent = 1 - player
    pawn_bits = bitscan(pos.bbs[player][Piece.PAWN])

    occupied = pos.get_player_occupied(player)
    opponent_occupied = pos.get_player_occupied(opponent)

    all_occupied = occupied | opponent_occupied

    move_list = []

    for pawn_bit in pawn_bits:
        all_pawn_advances = u64(PAWN_ADVANCE_MASKS[player][pawn_bit] & ~all_occupied)

        # Edge case: a pawn cannot advance two steps if obstructed
        if player == Player.WHITE and 8 <= pawn_bit <= 15:
            if all_occupied & u64((1 << (pawn_bit + 8))):
                all_pawn_advances &= u64(~(1 << (pawn_bit + 16)))
        elif player == Player.BLACK and 48 <= pawn_bit <= 55:
            if all_occupied & u64((1 << (pawn_bit - 8))):
                all_pawn_advances &= u64(~(1 << (pawn_bit - 16)))

        for dst_bit in bitscan(all_pawn_advances):
            flags = 0
            if abs(pawn_bit - dst_bit) == 16:
                flags |= DOUBLE_PAWN_PUSH
            move_list.append(encode_move(pawn_bit, dst_bit, Piece.PAWN, flags=flags))

        # A pawn can only capture a square if the enemy is there
        all_pawn_attacks = (
                PAWN_ATTACK_MASKS[player][pawn_bit] & opponent_occupied)

        for dst_bit in bitscan(all_pawn_attacks):
            for piece in Piece:
                if pos.bbs[opponent][piece] & u64(1 << dst_bit):
                    move_list.append(encode_move(pawn_bit, dst_bit, Piece.PAWN, captured=piece))
                    break

    return move_list


def generate_knight_moves(pos):
    player = pos.player_to_move
    opponent = 1 - player

    occupied = pos.get_player_occupied(player)
    opponent_occupied = pos.get_player_occupied(opponent)

    move_list = []
    knight_bits = bitscan(pos.bbs[player][Piece.KNIGHT])

    for knight_bit in knight_bits:
        all_knight_moves = KNIGHT_MOVE_MASKS[knight_bit]
        all_knight_moves &= ~occupied

        dsts = bitscan(all_knight_moves)

        for dst in dsts:
            if opponent_occupied & np.uint64(1 << dst):
                for piece in Piece:
                    if pos.bbs[opponent][piece] & np.uint64(1 << dst):
                        move_list.append(encode_move(knight_bit, dst, Piece.KNIGHT, captured=piece))
                        break
            else:
                move_list.append(encode_move(knight_bit, dst, Piece.KNIGHT))

    return move_list


def generate_bishop_moves(pos):
    return generate_sliding_moves(pos, Piece.BISHOP, [-9, -7, 7, 9])


def generate_rook_moves(pos):
    return generate_sliding_moves(pos, Piece.ROOK, [-8, -1, 1, 8])


def generate_queen_moves(pos):
    return generate_sliding_moves(pos, Piece.QUEEN,
                                  [-9, -8, -7, -1, 1, 7, 8, 9])


def generate_king_moves(pos: Position):
    player = pos.player_to_move

    # Get the unsafe squares that would put the king in check
    unsafe_squares = generate_king_unsafe_squares(pos, Player(player))

    # Get the bit that the current king to play is on
    king_bit = bitscan(pos.bbs[player][Piece.KING])[0]

    # Get the pseudolegal move bitboard for the king's current position
    all_king_moves = KING_MOVE_MASKS[king_bit]

    occupied = pos.get_player_occupied(player)
    opponent_occupied = pos.get_player_occupied(1 - player)

    new_moves = all_king_moves & ~occupied
    move_list = []
    for dst_bit in bitscan(new_moves):
        if opponent_occupied & (1 << dst_bit):
            for piece in Piece:
                if pos.bbs[1 - pos.player_to_move][piece] & (1 << dst_bit):
                    if not (unsafe_squares & u64(1 << dst_bit)):
                        move_list.append(encode_move(king_bit, dst_bit, Piece.KING, captured=piece))
                        break
        else:
            if not (unsafe_squares & u64(1 << dst_bit)):
                move_list.append(encode_move(king_bit, dst_bit, Piece.KING))

    return move_list


def generate_sliding_moves(pos: Position, piece: Piece, deltas: list[int]):
    player = pos.player_to_move
    opponent = 1 - player

    occupied = pos.get_player_occupied(player)
    opponent_occupied = pos.get_player_occupied(opponent)

    move_list = []
    piece_bits = bitscan(pos.bbs[player][piece])

    for piece_bit in piece_bits:
        piece_file = piece_bit % 8
        for d in deltas:
            last_file = piece_file
            for step in range(1, 8):
                dst_bit = piece_bit + d * step
                dst_file = dst_bit % 8

                # Stop at board bounds or own occupied square
                if not (0 <= dst_bit <= 63):
                    break
                if (d in (-9, -7, -1, 1, 7, 9) # check could be improved
                    and abs(dst_file - last_file) != 1):
                    break
                if occupied & (1 << dst_bit):
                    break
                
                #
                if (opponent_occupied & (1 << dst_bit)):
                    for candidate in Piece:
                        if pos.bbs[opponent][candidate] & np.uint64(1 << dst_bit):
                            move_list.append(encode_move(piece_bit, dst_bit, piece, captured=candidate))
                            break
                else:
                    move_list.append(encode_move(piece_bit, dst_bit, piece))
                last_file = dst_file

    return move_list


def is_in_check(p: Position, player: Player) -> bool:
    king_sq = bitscan(p.bbs[player][Piece.KING])[0]
    opponent = 1 - player

    original_player = p.player_to_move
    p.player_to_move = opponent
    opponent_moves = generate_moves(p)
    p.player_to_move = original_player

    for mv in opponent_moves:
        if get_to_sq(mv) == king_sq:
            return True
    return False


def generate_king_unsafe_squares(p: Position, player: Player) -> np.uint64:
    # Generates squares that are not legal for the king to move to on the next move.
    unsafe_squares = u64(0)
    
    # Unsafe squares is equal to all of the squares that could possibly be moved to by another piece, if the king could move there
    # Since we are likely only using this result to determine squares that the king cannot move to, the inclusion of the squares which
    # the player to move (king-mover) occupies with their pieces is not a big deal.
    # Performance of this is not super duper critical because it will only be used *when moving the king*.

    # We begin with the attacked squares by the pawns, knights, and enemy king:
    for piece in (Piece.PAWN, Piece.KNIGHT, Piece.KING):
        opp_sqs = bitscan(p.bbs[1 - player][piece])

        if piece == Piece.PAWN:
            # Masks for all pawn attacks by the enemy
            pawn_attack_masks = PAWN_ATTACK_MASKS[1 - player]
            for pawn_sq in opp_sqs:
                unsafe_squares |= pawn_attack_masks[pawn_sq]
        else:
            for opp_sq in opp_sqs:
                unsafe_squares |= NK_ATTACK_MASKS[piece][opp_sq]
    
    # Now, we need to iterate over each sliding piece (bishop, rook, queen) and generate the mask of squares it could attack next
    # move if the king moved there.

    # King can capture all squares with an enemy piece on them (except the king (do we account for this?))

    # Get board occupied squares
    opponent_occupied = p.get_player_occupied(1 - player)
    player_occupied_no_king = p.get_player_occupied(player) & ~p.bbs[player][Piece.KING]

    # For each sliding piece and the direction it moves in, we want to include one capture deep into any enemy players.
    # (Assuming the king captures a piece, we want to ensure that doing this will not put the king in check.)

    # The "attack ray" should not go past any non-opponent pieces (except for the king).

    # Exclude the king itself so that it doesn't block any unsafe squares behind itself (in the direction of the ray).

    deltas = {
        Piece.BISHOP: (-9, -7, 7, 9),
        Piece.ROOK: (-8, -1, 1, 8),
        Piece.QUEEN: (-9, -8, -7, -1, 1, 7, 8, 9)
    }

    # TODO: I want to shift directly instead of getting the bit number and then manipulating from there.
    for piece in (Piece.BISHOP, Piece.ROOK, Piece.QUEEN):
        piece_bits = bitscan(p.bbs[player][piece])
        for piece_bit in piece_bits:
            current_bit = piece_bit
            for d in deltas:
                for _ in range(1, 8):
                    dst_bit = current_bit + d

                    if (not (0 <= dst_bit <= 63) or player_occupied_no_king & (1 << dst_bit)
                        or (d in (-9, -7, -1, 1, 7, 9) and abs((current_bit % 8) - (dst_bit % 8)) > 1)):
                        break
                    else:
                        unsafe_squares |= u64(1 << dst_bit)

                        # If the target square is occupied by an opponent, stop here
                        if (opponent_occupied & u64(1 << dst_bit)):
                            break
                    
                    current_bit = dst_bit
    
    return unsafe_squares


def main():
    p = Position()
    moves = generate_legal_moves(p)
    print(p)
    print(f"Number of moves: {len(moves)}")
    print([move_to_string(m) for m in moves])

    print("MOVING FIRST MOVE")
    p.move(moves[1])

    moves = generate_legal_moves(p)
    print(p)
    print(f"Number of moves: {len(moves)}")
    print([move_to_string(m) for m in moves])

    p.move(moves[9])
    moves = generate_legal_moves(p)
    print(p)
    print(f"Number of moves: {len(moves)}")
    print([move_to_string(m) for m in moves])




if __name__ == "__main__":
    # Test out the move generation
    main()
