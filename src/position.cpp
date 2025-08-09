#include <string>
#include <map>

#include "castling_rights.h"
#include "chess_utils.h"
#include "move.h"
#include "piece.h"
#include "player.h"


struct Position {
    uint64_t bitboards[2][6]; // indexed with [player][piece]
    Player player_to_move;
    CastlingRights castling_rights[2];
    int halfmoves; // currently unused
    int fullmoves; // currently unused
    int ep_target; // currently unused
    int material_value[2];
    std::vector<uint32_t> move_stack;
    std::vector<CastlingRights> castling_rights_stack; // Push/pop 2 items at a time
};


Position init_position(std::string fen) {
    Position p;

    // Zero out the bitboards
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            p.bitboards[i][j] = 0ULL;
        }
    }

    // FENs are structured like
    // "[board] [side] [castling rights] [ep target] [halfmoves] [fullmoves]"

    // Split the FEN by spaces to get the individual components
    std::vector<std::string> fen_vec = split_str(fen, ' ');

    // Split the board by rank
    std::vector<std::string> board_vec = split_str(fen_vec[0], '/');

    // Iterate over the board vector
    int j = 8;
    int i = 56;

    for (int r = 0; r < 8; r++) {
        for (char c: board_vec[r]) {
            if ('1' <= c && c <= '8') {
                i += (c - '1' + 1);
            } else {
                p.bitboards[chtopl(c)][chtopc(c)] |= (uint64_t) 1 << i;
                i++;
            }
        }

        j--;
        i = (j - 1) * 8;
    }

    // Get the current player to move
    if (fen_vec[1] == "w") {
        p.player_to_move = Player::WHITE;
    } else if (fen_vec[1] == "b") {
        p.player_to_move = Player::BLACK;
    }

    // Zero out the castling rights
    for (int i = 0; i < 2; i++) {
        p.castling_rights[i] = (CastlingRights) 0;
    }

    for (char c: fen_vec[3]) {
        switch (c) {
            case 'k':
                p.castling_rights[Player::BLACK] |= (CastlingRights::KING | CastlingRights::KROOK);
            case 'q':
                p.castling_rights[Player::BLACK] |= (CastlingRights::KING | CastlingRights::QROOK);
            case 'K':
                p.castling_rights[Player:: WHITE] |= (CastlingRights::KING | CastlingRights::KROOK);
            case 'Q':
                p.castling_rights[Player::WHITE] |= (CastlingRights::KING | CastlingRights::QROOK);
        }
    }

    // Get the en passant target square (not used yet)
    if (fen_vec[3] == "-") {
        p.ep_target = 0ULL;
    } else {
        int file = (char) fen_vec[3][0] - (char) 'a';
        int rank = (char) fen_vec[3][1] - (char) '1';

        p.ep_target = rank * 8 + file;
    }

    p.halfmoves = std::stoi(fen_vec[4]);
    p.fullmoves = std::stoi(fen_vec[5]);

    p.material_value[0] = 0;
    p.material_value[1] = 0;

    // Calculate starting material value
    for (int pl = 0; pl < 2; pl++) {
        for (int pc = 0; pc < 6; pc++) {
            uint64_t tmp = p.bitboards[pl][pc];
            while (tmp) {
                int square = pop_lsb(tmp);
                p.material_value[pl] += piece_value((Piece) pc);
            }
        }
    }

    return p;
}


void move(Position *p, uint32_t move) {
    // MOVE DECODING
    uint32_t move_flags = get_flags(move);
    Piece promotion_piece = get_promotion(move);
    uint64_t from_bb = (uint64_t) 1 << get_from_sq(move);
    uint64_t to_bb = (uint64_t) 1 << get_to_sq(move);
    Piece piece_moved = get_piece(move);

    // Handle forfeiting castling rights due to movement of pieces
    if (piece_moved == Piece::KING) {
        p->castling_rights[p->player_to_move] &= ~(CastlingRights::KING);
    } else if (piece_moved == Piece::ROOK) {
        if (from_bb == (1ULL << (56 * p->player_to_move))) {
            p->castling_rights[p->player_to_move] &= ~CastlingRights::QROOK;
        } else if (from_bb == (1ULL << (7 + 56 * p->player_to_move))) {
            p->castling_rights[p->player_to_move] &= ~CastlingRights::KROOK;
        }
    }

    // CASTLES
    if (move_flags & (MoveFlags::CASTLE)) {
        int rook_from;
        int rook_to;
        if (move_flags & MoveFlags::KING_CASTLE) {
            // Special adjustment of the rook bitboard in the event of a castle
            rook_from = 7 + 56 * p->player_to_move;
            rook_to = 5 + 56 * p->player_to_move;

            p->castling_rights[p->player_to_move] &= ~(CastlingRights::KING | CastlingRights::KROOK);
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            rook_from = 0 + 56 * p->player_to_move;
            rook_to = 2 + 56 * p->player_to_move;

            p->castling_rights[p->player_to_move] &= ~(CastlingRights::KING | CastlingRights::QROOK);
        }
        p->bitboards[p->player_to_move][Piece::ROOK] &= ~(1ULL << rook_from);
        p->bitboards[p->player_to_move][Piece::ROOK] |=  (1ULL << rook_to);
    } else if (move_flags & MoveFlags::DOUBLE_PAWN_PUSH) {
        if (p->player_to_move == Player::WHITE) {
            // TODO: do we want to handle en passant this way?
        } else if (p->player_to_move == Player::BLACK) {
            // see above
        }
    }

    // CAPTURES
    if (move_flags & MoveFlags::CAPTURE) {
        Piece piece_captured = get_captured(move);
        p->material_value[1 - p->player_to_move] -= piece_value(piece_captured);

        if (!(move_flags & MoveFlags::EN_PASSANT)) {
            p->bitboards[1 - p->player_to_move][piece_captured] &= ~to_bb;
        } else {
            if (p->player_to_move == Player::WHITE) {
                p->bitboards[1 - p->player_to_move][Piece::PAWN] &= ~(to_bb >> 8);
            } else if (p->player_to_move == Player::BLACK) {
                p->bitboards[1 - p->player_to_move][Piece::PAWN] &= ~(to_bb << 8);
            }

            // If the captured piece was a rook in the corner, then opponent can no longer castle on that side
            if (piece_captured == Piece::ROOK) {
                if (to_bb == (1ULL << (56 * (1 - p->player_to_move)))) {
                    p->castling_rights[1 - p->player_to_move] &= ~(CastlingRights::QROOK);
                } else if (to_bb == (1ULL << (7 + 56 * (1 - p->player_to_move)))) {
                    p->castling_rights[1 - p->player_to_move] &= ~(CastlingRights::KROOK);
                }
            }
        }
    }

    // PROMOTIONS
    if (move_flags & (MoveFlags::PROMO)) {
        Piece promo_piece = get_promotion(move);
        p->bitboards[p->player_to_move][promo_piece] |= to_bb;
        p->material_value[p->player_to_move] += piece_value(promo_piece);
        p->material_value[p->player_to_move] -= piece_value(Piece::PAWN);
    } else {
        p->bitboards[p->player_to_move][piece_moved] |= to_bb;
    }

    // Remove the moved piece from the original square
    p->bitboards[p->player_to_move][piece_moved] &= ~from_bb;

    // Switch the turn
    p->player_to_move = Player(1 - p->player_to_move);

    // Handle move stack and castling rights stack
    p->move_stack.push_back(move);
    p->castling_rights_stack.push_back(p->castling_rights[Player::WHITE]);
    p->castling_rights_stack.push_back(p->castling_rights[Player::BLACK]);
}


void unmove(Position *p, uint32_t move) {
    p->player_to_move = (Player) (1 - p->player_to_move);

    // MOVE DECODING
    uint32_t move_flags = get_flags(move);
    Piece promotion_piece = get_promotion(move);
    uint64_t from_bb = (uint64_t) 1 << get_from_sq(move);
    uint64_t to_bb = (uint64_t) 1 << get_to_sq(move);
    Piece piece_moved = get_piece(move);

    // CASTLES
    if (move_flags & (MoveFlags::CASTLE)) {
        int rook_from;
        int rook_to;
        if (move_flags & MoveFlags::KING_CASTLE) {
            // Special adjustment of the rook bitboard in the event of a castle
            rook_from = 7 + 56 * p->player_to_move;
            rook_to = 5 + 56 * p->player_to_move;
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            rook_from = 0 + 56 * p->player_to_move;
            rook_to = 2 + 56 * p->player_to_move;
        }
        p->bitboards[p->player_to_move][Piece::ROOK] |= 1ULL << rook_from;
        p->bitboards[p->player_to_move][Piece::ROOK] &= ~(1ULL << rook_to);
    } else if (move_flags & MoveFlags::DOUBLE_PAWN_PUSH) {
        if (p->player_to_move == Player::WHITE) {
            // TODO: do we want to handle en passant this way?
        } else if (p->player_to_move == Player::BLACK) {
            // see above
        }
    }

    // CAPTURES
    if (move_flags & MoveFlags::CAPTURE) {
        Piece piece_captured = get_captured(move);
        p->material_value[1 - p->player_to_move] += piece_value(piece_captured);

        if (!(move_flags & MoveFlags::EN_PASSANT)) {
            p->bitboards[1 - p->player_to_move][piece_captured] |= to_bb;
        } else {
            if (p->player_to_move == Player::WHITE) {
                p->bitboards[1 - p->player_to_move][Piece::PAWN] |= to_bb >> 8;
            } else if (p->player_to_move == Player::BLACK) {
                p->bitboards[1 - p->player_to_move][Piece::PAWN] |= to_bb << 8;
            }
        }
    }

    // PROMOTIONS
    if (move_flags & (MoveFlags::PROMO)) {
        Piece promo_piece = get_promotion(move);
        p->bitboards[p->player_to_move][promo_piece] &= ~to_bb;
        p->material_value[p->player_to_move] -= piece_value(promo_piece);
        p->material_value[p->player_to_move] += piece_value(Piece::PAWN);
    } else {
        p->bitboards[p->player_to_move][piece_moved] &= ~to_bb;
    }

    // Add the moved piece back to the original square
    p->bitboards[p->player_to_move][piece_moved] |= from_bb;

    // Handle move stack and castling rights stack
    p->move_stack.pop_back();

    // Yes, I know this is kind of a weird way of handling the castling stack
    CastlingRights b_last_rights = p->castling_rights_stack.back();
    p->castling_rights_stack.pop_back();
    CastlingRights w_last_rights = p->castling_rights_stack.back();
    p->castling_rights_stack.pop_back();

    p->castling_rights[Player::BLACK] = b_last_rights;
    p->castling_rights[Player::WHITE] = w_last_rights;
}


uint64_t get_player_occupied(Position *p, int player) {
    uint64_t occ = 0ULL;
    for (int i = 0; i < 6; i++) {
        occ = occ | p->bitboards[player][i];
    }
    return occ;
}
