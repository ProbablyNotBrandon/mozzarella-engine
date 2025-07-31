import numpy as np
from chess_utils import *
from move import *
from move_masks import *
from position import Position, CastlingRights, Player, Piece, square_name
import time


PAWN_ADVANCE_MASKS = np.load("move_masks/pawn_advance_masks.npy")
PAWN_ATTACK_MASKS = np.load("move_masks/pawn_attack_masks.npy")
KNIGHT_MOVE_MASKS = np.load("move_masks/knight_move_masks.npy")
KING_MOVE_MASKS = np.load("move_masks/king_move_masks.npy")

NK_ATTACK_MASKS = {
    Piece.KNIGHT: KNIGHT_MOVE_MASKS,
    Piece.KING: KING_MOVE_MASKS
}


def generate_moves(p: Position):
    moves = []
    moves += generate_pawn_moves(p)
    moves += generate_knight_moves(p)
    moves += generate_bishop_moves(p)
    moves += generate_rook_moves(p)
    moves += generate_queen_moves(p)
    moves += generate_king_moves(p)
    moves += generate_castle_moves(p)

    return moves


def generate_legal_moves(p: Position, print_out=True):
    moves = []

    player = p.player_to_move
    in_check = is_in_check(p, player)

    moves += generate_pawn_moves(p)
    moves += generate_knight_moves(p)
    moves += generate_bishop_moves(p)
    moves += generate_rook_moves(p)
    moves += generate_queen_moves(p)
    # moves += generate_king_moves(p)
    castle_moves = generate_castle_moves(p) # These should be checked against all types of checks

    legal_moves = []
    for move in moves:
        p.move(move)
        if not is_in_sliding_check(p, Player(player)):
            legal_moves.append(move)
        p.unmove(move)

    # Cannot castle while in check
    if not in_check:
        for castle_move in castle_moves:
            p.move(castle_move)
            if not is_in_check(p, Player(player)):
                legal_moves.append(castle_move)
            p.unmove(castle_move)
    
    legal_moves += generate_legal_king_moves(p)

    if print_out:
        for i in range(len(legal_moves)):
            print(f"{i}: {Move(legal_moves[i])}")

    return legal_moves


def generate_castle_moves(p: Position):
    w_qs_castle_mask = np.uint64((1 << 1) | (1 << 2) | (1 << 3))
    w_ks_castle_mask = np.uint64((1 << 5) | (1 << 6))

    b_qs_castle_mask = np.uint64((1 << 57) | (1 << 58) | (1 << 59))
    b_ks_castle_mask = np.uint64((1 << 62) | (1 << 61))

    all_occupied = p.get_all_occupied()

    moves = []

    if p.player_to_move == Player.WHITE:
        if p.castling_rights & (CastlingRights.WHITE_KING | CastlingRights.WHITE_QROOK) and not (w_qs_castle_mask & all_occupied):
            moves.append(encode_move(4, 2, Piece.KING, flags=QUEEN_CASTLE))
        if p.castling_rights & (CastlingRights.WHITE_KING | CastlingRights.WHITE_KROOK) and not (w_ks_castle_mask & all_occupied):
            moves.append(encode_move(4, 6, Piece.KING, flags=KING_CASTLE))
    elif p.player_to_move == Player.BLACK:
        if p.castling_rights & (CastlingRights.BLACK_KING | CastlingRights.BLACK_QROOK) and not (b_qs_castle_mask & all_occupied):
            moves.append(encode_move(60, 58, Piece.KING, flags=QUEEN_CASTLE))
        if p.castling_rights & (CastlingRights.BLACK_KING | CastlingRights.BLACK_KROOK) and not (b_ks_castle_mask & all_occupied):
            moves.append(encode_move(60, 62, Piece.KING, flags=KING_CASTLE))

    return moves


def generate_pawn_moves(p):
    player = p.player_to_move
    opponent = 1 - player
    pawn_bits = bitscan(p.bbs[player][Piece.PAWN])

    occupied = p.get_player_occupied(player)
    opponent_occupied = p.get_player_occupied(opponent)

    all_occupied = occupied | opponent_occupied

    move_list = []

    for pawn_sq in pawn_bits:
        all_pawn_advances = PAWN_ADVANCE_MASKS[player][pawn_sq] & ~all_occupied

        # Edge case: a pawn cannot advance two steps if obstructed
        if player == Player.WHITE and 8 <= pawn_sq <= 15:
            if all_occupied & u64((1 << (pawn_sq + 8))):
                all_pawn_advances &= u64(~(1 << (pawn_sq + 16)))
        elif player == Player.BLACK and 48 <= pawn_sq <= 55:
            if all_occupied & u64((1 << (pawn_sq - 8))):
                all_pawn_advances &= u64(~(1 << (pawn_sq - 16)))

        for dst_sq in bitscan(all_pawn_advances):
            flags = 0
            if abs(pawn_sq - dst_sq) == 16:
                flags = DOUBLE_PAWN_PUSH
            move_list.append(encode_move(pawn_sq, dst_sq, Piece.PAWN, flags=flags))

        # A pawn can only capture a square if the enemy is there
        all_pawn_attacks = PAWN_ATTACK_MASKS[player][pawn_sq] & opponent_occupied

        for dst_sq in bitscan(all_pawn_attacks):
            flag = False
            for piece in Piece:
                if p.bbs[opponent][piece] & (u64(1) << u64(dst_sq)):
                    move_list.append(encode_move(pawn_sq, dst_sq, Piece.PAWN, captured=piece, flags=CAPTURE))
                    flag = True
                    break
            if not flag:
                print(f"PAWN CAPTURE ERROR: NO CAPTURE PIECE FOUND - SQ {bit_to_fr(dst_sq)}")

    return move_list


def generate_knight_moves(p):
    player = p.player_to_move
    opponent = 1 - player

    occupied = p.get_player_occupied(player)
    opponent_occupied = p.get_player_occupied(opponent)

    move_list = []
    knight_bits = bitscan(p.bbs[player][Piece.KNIGHT])

    for knight_bit in knight_bits:
        all_knight_moves = KNIGHT_MOVE_MASKS[knight_bit]
        all_knight_moves &= ~occupied

        dsts = bitscan(all_knight_moves)

        for dst in dsts:
            if opponent_occupied & np.uint64(1 << dst):
                flag = False
                for piece in Piece:
                    if p.bbs[opponent][piece] & np.uint64(1 << dst):
                        move_list.append(encode_move(knight_bit, dst, Piece.KNIGHT, captured=piece, flags=CAPTURE))
                        flag = True
                        break
                if not flag:
                    print(f"KNIGHT CAPTURE ERROR: NO CAPTURE PIECE FOUND  - SQ {bit_to_fr(dst)}")
            else:
                move_list.append(encode_move(knight_bit, dst, Piece.KNIGHT))

    return move_list


def generate_bishop_moves(p):
    return generate_sliding_moves(p, Piece.BISHOP, [-9, -7, 7, 9])


def generate_rook_moves(p):
    return generate_sliding_moves(p, Piece.ROOK, [-8, -1, 1, 8])


def generate_queen_moves(p):
    return generate_sliding_moves(p, Piece.QUEEN,
                                  [-9, -8, -7, -1, 1, 7, 8, 9])


def generate_legal_king_moves(p: Position):
    player = p.player_to_move

    # Get the unsafe squares that would put the king in check
    unsafe_squares = generate_king_unsafe_squares(p, Player(player))

    # Get the bit that the current king to play is on
    king_bit = bitscan(p.bbs[player][Piece.KING])[0]

    # Get the pseudolegal move bitboard for the king's current position
    all_king_moves = KING_MOVE_MASKS[king_bit]

    occupied = p.get_player_occupied(player)
    opponent_occupied = p.get_player_occupied(1 - player)

    new_moves = all_king_moves & ~occupied
    move_list = []
    for dst_bit in bitscan(new_moves):
        if opponent_occupied & u64(1 << dst_bit):
            flag = False
            pieces_checked = []
            for piece in Piece:
                pieces_checked.append(piece)
                if p.bbs[1 - p.player_to_move][piece] & (1 << dst_bit):
                    m = encode_move(king_bit, dst_bit, Piece.KING, captured=piece, flags=CAPTURE)
                    p.move(m)
                    if not is_in_check(p, player):
                        move_list.append(m)
                    p.unmove(m)

                    if not (unsafe_squares & u64(1 << dst_bit)):
                        move_list.append(encode_move(king_bit, dst_bit, Piece.KING, captured=piece, flags=CAPTURE))
                        flag = True
                        break
            if not flag:
                print(f"KING CAPTURE FAILURE {square_name(king_bit)}{square_name(dst_bit)}")
                generate_king_unsafe_squares(p, player, print_out=False)

        else:
            if not (unsafe_squares & u64(1 << dst_bit)):
                move_list.append(encode_move(king_bit, dst_bit, Piece.KING))

    return move_list


def generate_king_moves(p: Position):
    player = p.player_to_move

    king_sq = bitscan(p.bbs[player][Piece.KING])[0]
    king_move_mask = KING_MOVE_MASKS[king_sq] & ~(p.get_player_occupied(player))

    opp_occupied = p.get_player_occupied(1 - player)
    
    captures = king_move_mask & opp_occupied
    non_captures = king_move_mask & ~(opp_occupied)

    moves = []

    for to_sq in bitscan(captures):
        to_sq_mask = u64(1 << to_sq)
        flag = False
        for opp_piece in Piece:
            if to_sq_mask & p.bbs[1 - player][opp_piece]:
                moves.append(encode_move(king_sq, to_sq, Piece.KING, captured=opp_piece, flags=CAPTURE))
                flag = True
                break
        if not flag:
            print(f"KING NON-LEGAL CAPTURE ERROR: NO CAPTURE PIECE FOUND  - SQ {bit_to_fr(to_sq)}")

    for to_sq in bitscan(non_captures):
        moves.append(encode_move(king_sq, to_sq, Piece.KING))
    
    return moves


def generate_sliding_moves(p: Position, piece: Piece, deltas: list[int]):
    player = p.player_to_move
    opponent = 1 - player

    occupied = p.get_player_occupied(player)
    opponent_occupied = p.get_player_occupied(opponent)

    move_list = []
    piece_bits = bitscan(p.bbs[player][piece])

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
                    and abs(dst_file - last_file) > 1):
                    break
                if occupied & (1 << dst_bit):
                    break
                
                # Encode captured piece if applicable
                if (opponent_occupied & (u64(1) << u64(dst_bit))):
                    found_piece = False
                    for candidate in Piece:
                        if p.bbs[opponent][candidate] & (u64(1) << u64(dst_bit)):
                            found_piece = True
                            move_list.append(encode_move(piece_bit, dst_bit, piece, captured=candidate, flags=CAPTURE))
                            break
                    if not found_piece:
                            print("SLIDING CAPTURE ERROR: NO CAPTURE PIECE FOUND")
                    break
                else:
                    move_list.append(encode_move(piece_bit, dst_bit, piece))
                last_file = dst_file

    return move_list


def is_in_check(p: Position, player: Player) -> bool:
    king_sq = bitscan(p.bbs[player][Piece.KING])[0]
    king_file = king_sq % 8
    opponent = 1 - player

    # Check for knight attacks
    if KNIGHT_MOVE_MASKS[king_sq] & p.bbs[opponent][Piece.KNIGHT]:
        return True
    
    # Check for pawn attacks
    # TODO: Fix this ridiculous sh-
    # (Likely could just precalculate this)
    shift = lambda x, y: x >> y if player == Player.WHITE else y >> x
    pawn_attack_deltas = [7, 9][::(-1, 1)[player]]
    if king_file == 0:
        pawn_attack_deltas.remove(pawn_attack_deltas[0])
    if king_file == 7:
        pawn_attack_deltas.remove(pawn_attack_deltas[1])
    for d in pawn_attack_deltas:
        if shift(p.bbs[player][Piece.KING], d) & p.bbs[opponent][Piece.PAWN]:
            return True
    
    # Check for sliding piece checks
    return is_in_sliding_check(p, player)


def is_in_sliding_check(p: Position, player: Player) -> bool:
    king_sq = bitscan(p.bbs[player][Piece.KING])[0]
    king_file = king_sq % 8
    opponent = 1 - player
    occupied = p.get_player_occupied(player)

    # Check for sliding piece checks
    opp_bishops_and_queens = p.bbs[opponent][Piece.BISHOP] | p.bbs[opponent][Piece.QUEEN]
    opp_rooks_and_queens = p.bbs[opponent][Piece.ROOK] | p.bbs[opponent][Piece.QUEEN]

    for deltas, opp_mask in (((-9, -7, 7, 9), opp_bishops_and_queens), ((-8, -1, 1, 8), opp_rooks_and_queens)):
        for d in deltas:
            curr_file = king_file
            for step in range(1, 8):
                to_sq = king_sq + d * step
                to_file = to_sq % 8
                
                if not (0 <= to_sq <= 63):
                    break
                if abs(to_file - curr_file) > 1:
                    break
                if occupied & (u64(1) << u64(to_sq)):
                    break
                
                if opp_mask & (u64(1) << u64(to_sq)):
                    return True
                
                curr_file = to_file

    return False


def generate_king_unsafe_squares(p: Position, player: Player, print_out=False) -> np.uint64:
    # Generates squares that are not legal for the king to move to on the next move.
    unsafe_squares = u64(0)

    unsafe_squares_list = [u64(0)] * 6
    
    # Pawn, knight, and king attacked squares
    for piece in (Piece.PAWN, Piece.KNIGHT, Piece.KING):
        opp_sqs = bitscan(p.bbs[1 - player][piece])

        if piece == Piece.PAWN:
            # Masks for all pawn attacks by the enemy
            pawn_attack_masks = PAWN_ATTACK_MASKS[1 - player]
            for pawn_sq in opp_sqs:
                unsafe_squares_list[Piece.PAWN] |= pawn_attack_masks[pawn_sq]
        else:
            for opp_sq in opp_sqs:
                unsafe_squares_list[piece] |= NK_ATTACK_MASKS[piece][opp_sq]
    
    
    # Get board occupied squares
    opponent_occupied = p.get_player_occupied(1 - player)
    player_occupied_no_king = p.get_player_occupied(player) & ~p.bbs[player][Piece.KING]

    # Sliding piece attacked squares (skip through the king in the attack rays)
    deltas = {
        Piece.BISHOP: (-9, -7, 7, 9),
        Piece.ROOK: (-8, -1, 1, 8),
        Piece.QUEEN: (-9, -8, -7, -1, 1, 7, 8, 9)
    }

    # TODO: shift bitboards directly instead of a square number
    for piece in (Piece.BISHOP, Piece.ROOK, Piece.QUEEN):
        piece_bits = bitscan(p.bbs[1 - player][piece])
        for piece_bit in piece_bits:
            for d in deltas[piece]:
                current_bit = piece_bit
                for _ in range(1, 8):
                    dst_bit = current_bit + d

                    if (not (0 <= dst_bit <= 63) or player_occupied_no_king & (1 << dst_bit)
                        or (d in (-9, -7, -1, 1, 7, 9) and abs((current_bit % 8) - (dst_bit % 8)) > 1)):
                        break
                    else:
                        
                        unsafe_squares_list[piece] |= (u64(1) << u64(dst_bit))
                        #render_bitboard(unsafe_squares_list[piece])

                        # If the target square is occupied by an opponent, stop here
                        if (opponent_occupied & (u64(1) << u64(dst_bit))):
                            break
                    
                    current_bit = dst_bit
    if not print_out:
        for board in unsafe_squares_list:
            unsafe_squares |= board
    if print_out:
        print("==== UNSAFE SQUARE RESULT ====")
        print(p)
        for piece in Piece:
            print(f"{Piece(piece)} UNSAFE SQUARES:")
            render_bitboard(unsafe_squares_list[piece])
            unsafe_squares |= unsafe_squares_list[piece]
            #render_bitboard(unsafe_squares)
        print("==============================")
    
    return unsafe_squares


def render_bitboard(bb, origin_sq=None, symbol='x'):
    board = [['.' for _ in range(8)] for _ in range(8)]
    for i in range(64):
        if (bb >> i) & 1:
            row = 7 - (i // 8)
            col = i % 8
            board[row][col] = symbol
    if origin_sq is not None:
        row = 7 - (origin_sq // 8)
        col = origin_sq % 8
        board[row][col] = 'P'
    for row in board:
        print(' '.join(row))
    print()

def print_all_pawn_masks(player):
    print("=== PAWN ADVANCE MASKS ===")
    for sq in range(64):
        print(f"Square {sq} ({bit_to_fr(sq)}):")
        render_bitboard(PAWN_ADVANCE_MASKS[player][sq], origin_sq=sq, symbol='a')

    print("=== PAWN ATTACK MASKS ===")
    for sq in range(64):
        print(f"Square {sq} ({bit_to_fr(sq)}):")
        render_bitboard(PAWN_ATTACK_MASKS[player][sq], origin_sq=sq, symbol='x')