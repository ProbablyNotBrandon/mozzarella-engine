from position import Position, CastlingRights, Player, Piece
from move_masks import *
from move import *
from chessutils import *


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


def generate_castle_moves(pos: Position):
    w_qs_castle_mask = np.uint64((1 << 1) | (1 << 2) | (1 << 3))
    w_ks_castle_mask = np.uint64((1 << 5) | (1 << 6))

    b_qs_castle_mask = np.uint64((1 << 57) | (1 << 58) | (1 << 59))
    b_ks_castle_mask = np.uint64((1 << 62) | (1 << 61))

    all_occupied = pos.get_all_occupied()

    moves = []

    if pos.player_to_move == Player.WHITE:
        # for binary in (w_qs_castle_mask, w_ks_castle_mask, all_occupied):
        #     print(bin(int(binary)))
        if pos.castling_rights & CastlingRights.W_QSIDE and not (w_qs_castle_mask & all_occupied):
            moves.append(encode_move(4, 6, Piece.KING, flags=QUEEN_CASTLE))
        if pos.castling_rights & CastlingRights.W_KSIDE and not (w_ks_castle_mask & all_occupied):
            moves.append(encode_move(4, 1, Piece.KING, flags=KING_CASTLE))
    elif pos.player_to_move == Player.BLACK:
        if pos.castling_rights & CastlingRights.B_QSIDE and not (b_qs_castle_mask & all_occupied):
            moves.append(encode_move(4, 1, Piece.KING, flags=QUEEN_CASTLE))
        if pos.castling_rights & CastlingRights.B_KSIDE and not (b_ks_castle_mask & all_occupied):
            moves.append(encode_move(4, 1, Piece.KING, flags=KING_CASTLE))

    return moves


def generate_pawn_moves(pos):
    player = pos.player_to_move
    opponent = 1 - player
    pawn_bits = bitscan(pos.bbs[player][Piece.PAWN])

    occupied = np.uint64(0)
    for bb in pos.bbs[player]:
        occupied |= bb

    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[opponent]:
        opponent_occupied |= bb

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
            move_list.append(encode_move(pawn_bit, dst_bit, Piece.PAWN))

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

    occupied = np.uint64(0)
    for bb in pos.bbs[player]:
        occupied |= bb

    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[opponent]:
        opponent_occupied |= bb

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

    # Get the bit that the current king to play is on
    king_bit = bitscan(pos.bbs[player][Piece.KING])[0]

    # Get the pseudolegal move bitboard for the king's current position
    all_king_moves = KING_MOVE_MASKS[king_bit]
    occupied = np.uint64(0)
    for bb in pos.bbs[pos.player_to_move]:
        occupied |= bb
    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[1 - pos.player_to_move]:
        opponent_occupied |= bb

    new_moves = all_king_moves & ~occupied
    move_list = []
    for dst_bit in bitscan(new_moves):
        if opponent_occupied & (1 << dst_bit):
            for piece in Piece:
                if pos.bbs[1 - pos.player_to_move][piece] & (1 << dst_bit):
                    move_list.append(encode_move(king_bit, dst_bit, Piece.KING, captured=piece))
                    break
        else:
            move_list.append(encode_move(king_bit, dst_bit, Piece.KING))

    return move_list


def generate_sliding_moves(pos: Position, piece: Piece, deltas: list[int]):
    player = pos.player_to_move
    opponent = 1 - player

    occupied = np.uint64(0)
    for bb in pos.bbs[player]:
        occupied |= bb

    opponent_occupied = np.uint64(0)
    for bb in pos.bbs[opponent]:
        opponent_occupied |= bb

    move_list = []
    piece_bits = bitscan(pos.bbs[player][piece])

    for piece_bit in piece_bits:
        current_bit = piece_bit
        for d in deltas:
            for _ in range(1, 8):
                dst_bit = current_bit + d

                if not (0 <= dst_bit <= 63) or (occupied & (1 << dst_bit)):
                    break
                if (d in [-9, -7, -1, 1, 7, 9]
                    and abs((current_bit % 8) - (dst_bit % 8)) > 1):
                    break
                else:
                    if (opponent_occupied & (1 << dst_bit)):
                        for candidate in Piece:
                            if pos.bbs[opponent][candidate] & np.uint64(1 << dst_bit):
                                move_list.append(encode_move(piece_bit, dst_bit, piece, captured=candidate))
                                break
                        break
                    move_list.append(encode_move(piece_bit, dst_bit, piece))

                current_bit = dst_bit

    return move_list


def main():
    p = Position()
    moves = generate_moves(p)
    print(p)
    print("Number of moves: {len(moves)}")
    print([move_to_string(m) for m in moves])


if __name__ == "__main__":
    # Test out the move generation
    main()
