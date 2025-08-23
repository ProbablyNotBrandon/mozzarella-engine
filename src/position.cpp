#include "position.h"

uint64_t _ZOBRIST_ARR[781];


Position init_position(std::string fen) {
    init_zobrist();
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

    for (char c: fen_vec[2]) {
        switch (c) {
            case 'k':
                p.castling_rights[Player::BLACK] |= CastlingRights::KING_UNMOVED;
                p.castling_rights[Player::BLACK] |= CastlingRights::KROOK;
                break;
            case 'q':
                p.castling_rights[Player::BLACK] |= CastlingRights::KING_UNMOVED;
                p.castling_rights[Player::BLACK] |= CastlingRights::QROOK;
                break;
            case 'K':
                p.castling_rights[Player::WHITE] |= CastlingRights::KING_UNMOVED;
                p.castling_rights[Player::WHITE] |= CastlingRights::KROOK;
                break;
            case 'Q':
                p.castling_rights[Player::WHITE] |= CastlingRights::KING_UNMOVED;
                p.castling_rights[Player::WHITE] |= CastlingRights::QROOK;
                break;
        }
    }

    p.castling_rights_stack.push_back(p.castling_rights[Player::WHITE]);
    p.castling_rights_stack.push_back(p.castling_rights[Player::BLACK]);

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
    p.pst_eval = 0;

    // Calculate starting material value
    for (int pl = 0; pl < 2; pl++) {
        for (int pc = 0; pc < 5; pc++) {
            uint64_t tmp = p.bitboards[pl][pc];
            while (tmp) {
                int sq = pop_lsb(tmp);
                p.material_value[pl] += piece_value((Piece) pc);
                p.pst_eval += PST[pl][pc][sq];
            }
        }
    }

    p.pst_eval_stack.push_back(p.pst_eval);

    return p;
}


void move(Position *p, uint32_t move) {
    // MOVE DECODING
    uint32_t move_flags = get_flags(move);
    uint64_t from_bb = 1ULL << get_from_sq(move);
    uint64_t to_bb = 1ULL << get_to_sq(move);

    if (from_bb & to_bb) return;

    Piece piece_moved = get_piece(move);


    // Handle move stack and castling rights stack
    p->move_stack.push_back(move);
    p->castling_rights_stack.push_back(p->castling_rights[Player::WHITE]);
    p->castling_rights_stack.push_back(p->castling_rights[Player::BLACK]);

    // Handle eval stack
    p->pst_eval_stack.push_back(p->pst_eval);

    // Update PST, removing the value from the starting square
    if (piece_moved != Piece::KING) {
        p->pst_eval -= PST[p->player_to_move][piece_moved][get_from_sq(move)];
    }

    // Handle forfeiting castling rights due to movement of pieces
    if (piece_moved == Piece::KING) {
        p->castling_rights[p->player_to_move] &= ~(CastlingRights::KING_UNMOVED);
    } else if (piece_moved == Piece::ROOK) {
        if (from_bb == (1ULL << (56 * p->player_to_move))) {
            p->castling_rights[p->player_to_move] &= ~CastlingRights::QROOK;
        } else if (from_bb == (1ULL << (7 + 56 * p->player_to_move))) {
            p->castling_rights[p->player_to_move] &= ~CastlingRights::KROOK;
        }
    }

    // CASTLES
    if (move_flags & (MoveFlags::CASTLE)) {
        // int king_from = 4 + 56 * p->player_to_move;
        // int king_to;
        int rook_from = 0;
        int rook_to = 0;

        if (move_flags & MoveFlags::KING_CASTLE) {
            // king_to = king_from + 2;
            rook_from = 7 + 56 * p->player_to_move;
            rook_to = 5 + 56 * p->player_to_move;

            p->castling_rights[p->player_to_move] &= ~(CastlingRights::KING_UNMOVED | CastlingRights::KROOK);
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            // king_to = king_from - 2;
            rook_from = 0 + 56 * p->player_to_move;
            rook_to = 3 + 56 * p->player_to_move;

            p->castling_rights[p->player_to_move] &= ~(CastlingRights::KING_UNMOVED | CastlingRights::QROOK);
        }

        // Move king
        // p->pst_eval -= PST[p->player_to_move][Piece::KING][king_from];
        // p->pst_eval += PST[p->player_to_move][Piece::KING][king_to];

        // Move rook
        p->bitboards[p->player_to_move][Piece::ROOK] &= ~(1ULL << rook_from);
        p->bitboards[p->player_to_move][Piece::ROOK] |=  (1ULL << rook_to);
        p->pst_eval -= PST[p->player_to_move][Piece::ROOK][rook_from];
        p->pst_eval += PST[p->player_to_move][Piece::ROOK][rook_to];

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

            p->pst_eval -= PST[1 - p->player_to_move][piece_captured][get_to_sq(move)];

            // If the captured piece was a rook in the corner, then opponent can no longer castle on that side
            if (piece_captured == Piece::ROOK) {
                if (to_bb == (1ULL << (56 * (1 - p->player_to_move)))) {
                    p->castling_rights[1 - p->player_to_move] &= ~(CastlingRights::QROOK);
                } else if (to_bb == (1ULL << (7 + 56 * (1 - p->player_to_move)))) {
                    p->castling_rights[1 - p->player_to_move] &= ~(CastlingRights::KROOK);
                }
            }
        } else {
            int captured_sq = (p->player_to_move == Player::WHITE) ? get_to_sq(move) - 8 : get_to_sq(move) + 8;
            p->pst_eval -= PST[1 - p->player_to_move][Piece::PAWN][captured_sq];
            if (p->player_to_move == Player::WHITE) {
                p->bitboards[1 - p->player_to_move][Piece::PAWN] &= ~(to_bb >> 8);
            } else if (p->player_to_move == Player::BLACK) {
                p->bitboards[1 - p->player_to_move][Piece::PAWN] &= ~(to_bb << 8);
            }
        }
    }

    // PROMOTIONS
    if (move_flags & (MoveFlags::PROMO)) {
        Piece promo_piece = get_promotion(move);

        p->bitboards[p->player_to_move][promo_piece] |= to_bb;

        // Material update
        p->material_value[p->player_to_move] += piece_value(promo_piece) -  piece_value(Piece::PAWN);

        // PST update
        p->pst_eval += PST[p->player_to_move][promo_piece][get_to_sq(move)];
        p->pst_eval -= PST[p->player_to_move][Piece::PAWN][get_from_sq(move)];

    } else {
        p->bitboards[p->player_to_move][piece_moved] |= to_bb;

        if (piece_moved != Piece::KING) {
            p->pst_eval += PST[p->player_to_move][piece_moved][get_to_sq(move)];
        }
    }

    // Remove the moved piece from the original square
    p->bitboards[p->player_to_move][piece_moved] &= ~from_bb;

    // Switch the turn
    p->player_to_move = Player(1 - p->player_to_move);
}


void unmove(Position *p, uint32_t move) {
    p->player_to_move = (Player) (1 - p->player_to_move);

    // MOVE DECODING
    uint32_t move_flags = get_flags(move);
    uint64_t from_bb = (uint64_t) 1 << get_from_sq(move);
    uint64_t to_bb = (uint64_t) 1 << get_to_sq(move);
    if (from_bb & to_bb) return;
    Piece piece_moved = get_piece(move);

    // CASTLES
    if (move_flags & (MoveFlags::CASTLE)) {
        int rook_from = 0;
        int rook_to = 0;
        if (move_flags & MoveFlags::KING_CASTLE) {
            // Special adjustment of the rook bitboard in the event of a castle
            rook_from = 7 + 56 * p->player_to_move;
            rook_to = 5 + 56 * p->player_to_move;
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            rook_from = 0 + 56 * p->player_to_move;
            rook_to = 3 + 56 * p->player_to_move;
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

        // Material restore
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

        // Material restore
        p->material_value[p->player_to_move] -= piece_value(promo_piece) - piece_value(Piece::PAWN);

    } else {
        p->bitboards[p->player_to_move][piece_moved] &= ~to_bb;
    }

    // Add the moved piece back to the original square
    p->bitboards[p->player_to_move][piece_moved] |= from_bb;

    // Handle move stack and castling rights stack
    p->move_stack.pop_back();

    // Pop the last eval
    p->pst_eval = p->pst_eval_stack.back();
    p->pst_eval_stack.pop_back();

    // Yes, I know this is kind of a weird way of handling the castling stack
    uint8_t b_last_rights = p->castling_rights_stack.back();
    p->castling_rights_stack.pop_back();
    uint8_t w_last_rights = p->castling_rights_stack.back();
    p->castling_rights_stack.pop_back();

    p->castling_rights[Player::BLACK] = b_last_rights;
    p->castling_rights[Player::WHITE] = w_last_rights;
}


uint64_t get_occupied(Position *p, int player) {
    uint64_t occ = 0ULL;
    for (int i = 0; i < 6; i++) {
        occ = occ | p->bitboards[player][i];
    }
    return occ;
}

void init_zobrist() {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist;
    for (int i = 0; i < 781; i++) {
        _ZOBRIST_ARR[i] = dist(rng);
    }

}

uint64_t zobrist(Position *p) {

    uint64_t z = 0ULL;

    // XOR the values for each piece on each square of the board
    for (int i = 0; i < 12; i++) {
        uint64_t bb = p->bitboards[i / 6][i % 6];
        while (bb) {
            int sq = pop_lsb(bb); 
            z ^= _ZOBRIST_ARR[i + sq * 12];
        }
    }

    // XOR the values for each castling right
    int wcr = p->castling_rights[Player::WHITE];
    int bcr = p->castling_rights[Player::BLACK];

    if (wcr & CastlingRights::KING_UNMOVED) {
        if (wcr & CastlingRights::KROOK) z ^= _ZOBRIST_ARR[768];
        if (wcr & CastlingRights::QROOK) z ^= _ZOBRIST_ARR[769];
    }

    if (bcr & CastlingRights::KING_UNMOVED) {
        if (bcr & CastlingRights::KROOK) z ^= _ZOBRIST_ARR[770];
        if (bcr & CastlingRights::QROOK) z ^= _ZOBRIST_ARR[771];
    }

    // Include the EP target in the hash
    if (p->ep_target != -1) {
        int ep_file = p->ep_target % 8;

        if ((ep_file > 0 && p->bitboards[p->player_to_move][Piece::PAWN] & (1ULL << (p->ep_target - 1))) ||
            (ep_file < 7 && p->bitboards[p->player_to_move][Piece::PAWN] & (1ULL << (p->ep_target + 1)))) {
            z ^= _ZOBRIST_ARR[772 + ep_file];
        }
    }
    
    // Include the player to move in the hash
    if (p->player_to_move == Player::BLACK) z ^= _ZOBRIST_ARR[780];

    return z;

}
