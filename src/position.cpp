#include "position.h"

uint64_t _ZOBRIST_ARR[781];

bool ZOB_INIT = false;


/*
 * Constructor for a Position struct. Sets up the initial board state and data parameters.
 * If given a FEN string 
 */
Position::Position(std::string fen) {
    if (!ZOB_INIT) {
        ZOB_INIT = true;
        init_zobrist();
    }

    // Zero out the bitboards
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            this->bitboards[i][j] = 0ULL;
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
                this->bitboards[chtopl(c)][chtopc(c)] |= (uint64_t) 1 << i;
                i++;
            }
        }

        j--;
        i = (j - 1) * 8;
    }

    // Get the current player to move
    if (fen_vec[1] == "w") {
        this->player_to_move = Player::WHITE;
    } else if (fen_vec[1] == "b") {
        this->player_to_move = Player::BLACK;
    }

    // Zero out the castling rights
    for (int i = 0; i < 2; i++) {
        this->castling_rights[i] = (CastlingRights) 0;
    }

    for (char c: fen_vec[2]) {
        switch (c) {
            case 'k':
                this->castling_rights[Player::BLACK] |= CastlingRights::KING_UNMOVED;
                this->castling_rights[Player::BLACK] |= CastlingRights::KROOK;
                break;
            case 'q':
                this->castling_rights[Player::BLACK] |= CastlingRights::KING_UNMOVED;
                this->castling_rights[Player::BLACK] |= CastlingRights::QROOK;
                break;
            case 'K':
                this->castling_rights[Player::WHITE] |= CastlingRights::KING_UNMOVED;
                this->castling_rights[Player::WHITE] |= CastlingRights::KROOK;
                break;
            case 'Q':
                this->castling_rights[Player::WHITE] |= CastlingRights::KING_UNMOVED;
                this->castling_rights[Player::WHITE] |= CastlingRights::QROOK;
                break;
        }
    }

    this->castling_rights_stack.push_back(this->castling_rights[Player::WHITE]);
    this->castling_rights_stack.push_back(this->castling_rights[Player::BLACK]);

    // Get the en passant target square (not used yet)
    if (fen_vec[3] == "-") {
        this->ep_target = 0ULL;
    } else {
        int file = (char) fen_vec[3][0] - (char) 'a';
        int rank = (char) fen_vec[3][1] - (char) '1';

        this->ep_target = rank * 8 + file;
    }

    this->halfmoves = std::stoi(fen_vec[4]);
    this->fullmoves = std::stoi(fen_vec[5]);

    this->material_value[0] = 0;
    this->material_value[1] = 0;
    this->pst_eval = 0;

    // Calculate starting material value
    for (int pl = 0; pl < 2; pl++) {
        for (int pc = 0; pc < 5; pc++) {
            uint64_t tmp = this->bitboards[pl][pc];
            while (tmp) {
                int sq = pop_lsb(tmp);
                this->material_value[pl] += piece_value((Piece) pc);
                this->pst_eval += PST[pl][pc][sq];
            }
        }
    }

    this->pst_eval_stack.push_back(this->pst_eval);
}


/*
 * Make the requested move on the board. A move is encoded in a 32-bit integer.
 * Updates applicable board state.
 *
 * Note: this method assumes that the move itself is legal.
 * Legality checking is performed elsewhere.
 */
void Position::move(uint32_t move) {
    // MOVE DECODING
    uint32_t move_flags = get_flags(move);
    uint64_t from_bb = 1ULL << get_from_sq(move);
    uint64_t to_bb = 1ULL << get_to_sq(move);

    if (from_bb & to_bb) return;

    Piece piece_moved = get_piece(move);


    // Handle move stack and castling rights stack
    this->move_stack.push_back(move);
    this->castling_rights_stack.push_back(this->castling_rights[Player::WHITE]);
    this->castling_rights_stack.push_back(this->castling_rights[Player::BLACK]);

    // Handle eval stack
    this->pst_eval_stack.push_back(this->pst_eval);

    // Update PST, removing the value from the starting square
    if (piece_moved != Piece::KING) {
        this->pst_eval -= PST[this->player_to_move][piece_moved][get_from_sq(move)];
    }

    // Handle forfeiting castling rights due to movement of pieces
    if (piece_moved == Piece::KING) {
        this->castling_rights[this->player_to_move] &= ~(CastlingRights::KING_UNMOVED);
    } else if (piece_moved == Piece::ROOK) {
        if (from_bb == (1ULL << (56 * this->player_to_move))) {
            this->castling_rights[this->player_to_move] &= ~CastlingRights::QROOK;
        } else if (from_bb == (1ULL << (7 + 56 * this->player_to_move))) {
            this->castling_rights[this->player_to_move] &= ~CastlingRights::KROOK;
        }
    }

    // CASTLES
    if (move_flags & (MoveFlags::CASTLE)) {
        // int king_from = 4 + 56 * this->player_to_move;
        // int king_to;
        int rook_from = 0;
        int rook_to = 0;

        if (move_flags & MoveFlags::KING_CASTLE) {
            // king_to = king_from + 2;
            rook_from = 7 + 56 * this->player_to_move;
            rook_to = 5 + 56 * this->player_to_move;

            this->castling_rights[this->player_to_move] &= ~(CastlingRights::KING_UNMOVED | CastlingRights::KROOK);
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            // king_to = king_from - 2;
            rook_from = 0 + 56 * this->player_to_move;
            rook_to = 3 + 56 * this->player_to_move;

            this->castling_rights[this->player_to_move] &= ~(CastlingRights::KING_UNMOVED | CastlingRights::QROOK);
        }

        // Move king
        // this->pst_eval -= PST[this->player_to_move][Piece::KING][king_from];
        // this->pst_eval += PST[this->player_to_move][Piece::KING][king_to];

        // Move rook
        this->bitboards[this->player_to_move][Piece::ROOK] &= ~(1ULL << rook_from);
        this->bitboards[this->player_to_move][Piece::ROOK] |=  (1ULL << rook_to);
        this->pst_eval -= PST[this->player_to_move][Piece::ROOK][rook_from];
        this->pst_eval += PST[this->player_to_move][Piece::ROOK][rook_to];

    } else if (move_flags & MoveFlags::DOUBLE_PAWN_PUSH) {
        if (this->player_to_move == Player::WHITE) {
            // TODO: do we want to handle en passant this way?
        } else if (this->player_to_move == Player::BLACK) {
            // see above
        }
    }

    // CAPTURES
    if (move_flags & MoveFlags::CAPTURE) {
        Piece piece_captured = get_captured(move);
        this->material_value[1 - this->player_to_move] -= piece_value(piece_captured);

        if (!(move_flags & MoveFlags::EN_PASSANT)) {
            this->bitboards[1 - this->player_to_move][piece_captured] &= ~to_bb;

            this->pst_eval -= PST[1 - this->player_to_move][piece_captured][get_to_sq(move)];

            // If the captured piece was a rook in the corner, then opponent can no longer castle on that side
            if (piece_captured == Piece::ROOK) {
                if (to_bb == (1ULL << (56 * (1 - this->player_to_move)))) {
                    this->castling_rights[1 - this->player_to_move] &= ~(CastlingRights::QROOK);
                } else if (to_bb == (1ULL << (7 + 56 * (1 - this->player_to_move)))) {
                    this->castling_rights[1 - this->player_to_move] &= ~(CastlingRights::KROOK);
                }
            }
        } else {
            int captured_sq = (this->player_to_move == Player::WHITE) ? get_to_sq(move) - 8 : get_to_sq(move) + 8;
            this->pst_eval -= PST[1 - this->player_to_move][Piece::PAWN][captured_sq];
            if (this->player_to_move == Player::WHITE) {
                this->bitboards[1 - this->player_to_move][Piece::PAWN] &= ~(to_bb >> 8);
            } else if (this->player_to_move == Player::BLACK) {
                this->bitboards[1 - this->player_to_move][Piece::PAWN] &= ~(to_bb << 8);
            }
        }
    }

    // PROMOTIONS
    if (move_flags & (MoveFlags::PROMO)) {
        Piece promo_piece = get_promotion(move);

        this->bitboards[this->player_to_move][promo_piece] |= to_bb;

        // Material update
        this->material_value[this->player_to_move] += piece_value(promo_piece) -  piece_value(Piece::PAWN);

        // PST update
        this->pst_eval += PST[this->player_to_move][promo_piece][get_to_sq(move)];
        this->pst_eval -= PST[this->player_to_move][Piece::PAWN][get_from_sq(move)];

    } else {
        this->bitboards[this->player_to_move][piece_moved] |= to_bb;

        if (piece_moved != Piece::KING) {
            this->pst_eval += PST[this->player_to_move][piece_moved][get_to_sq(move)];
        }
    }

    // Remove the moved piece from the original square
    this->bitboards[this->player_to_move][piece_moved] &= ~from_bb;

    // Switch the turn
    this->player_to_move = Player(1 - this->player_to_move);
}


/*
 * Un-makes the requested move on the board. A move is encoded in a 32-bit integer.
 * Updates applicable board state.
 *
 * Note: this method assumes that the un-move itself is legal and was the most recent move.
 * Legality checking is performed elsewhere.
 */
void Position::unmove(uint32_t move) {
    this->player_to_move = (Player) (1 - this->player_to_move);

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
            rook_from = 7 + 56 * this->player_to_move;
            rook_to = 5 + 56 * this->player_to_move;
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            rook_from = 0 + 56 * this->player_to_move;
            rook_to = 3 + 56 * this->player_to_move;
        }
        this->bitboards[this->player_to_move][Piece::ROOK] |= 1ULL << rook_from;
        this->bitboards[this->player_to_move][Piece::ROOK] &= ~(1ULL << rook_to);
    } else if (move_flags & MoveFlags::DOUBLE_PAWN_PUSH) {
        if (this->player_to_move == Player::WHITE) {
            // TODO: do we want to handle en passant this way?
        } else if (this->player_to_move == Player::BLACK) {
            // see above
        }
    }

    // CAPTURES
    if (move_flags & MoveFlags::CAPTURE) {
        Piece piece_captured = get_captured(move);

        // Material restore
        this->material_value[1 - this->player_to_move] += piece_value(piece_captured);

        if (!(move_flags & MoveFlags::EN_PASSANT)) {
            this->bitboards[1 - this->player_to_move][piece_captured] |= to_bb;
        } else {
            if (this->player_to_move == Player::WHITE) {
                this->bitboards[1 - this->player_to_move][Piece::PAWN] |= to_bb >> 8;
            } else if (this->player_to_move == Player::BLACK) {
                this->bitboards[1 - this->player_to_move][Piece::PAWN] |= to_bb << 8;
            }
        }
    }

    // PROMOTIONS
    if (move_flags & (MoveFlags::PROMO)) {
        Piece promo_piece = get_promotion(move);
        this->bitboards[this->player_to_move][promo_piece] &= ~to_bb;

        // Material restore
        this->material_value[this->player_to_move] -= piece_value(promo_piece) - piece_value(Piece::PAWN);

    } else {
        this->bitboards[this->player_to_move][piece_moved] &= ~to_bb;
    }

    // Add the moved piece back to the original square
    this->bitboards[this->player_to_move][piece_moved] |= from_bb;

    // Handle move stack and castling rights stack
    this->move_stack.pop_back();

    // Pop the last eval
    this->pst_eval = this->pst_eval_stack.back();
    this->pst_eval_stack.pop_back();

    // Yes, I know this is kind of a weird way of handling the castling stack
    uint8_t b_last_rights = this->castling_rights_stack.back();
    this->castling_rights_stack.pop_back();
    uint8_t w_last_rights = this->castling_rights_stack.back();
    this->castling_rights_stack.pop_back();

    this->castling_rights[Player::BLACK] = b_last_rights;
    this->castling_rights[Player::WHITE] = w_last_rights;
}


/*
 * Gets the occupied squares of a given player in the form of a bitboard.
 */
uint64_t Position::get_occupied(int player) {
    uint64_t occ = 0ULL;
    for (int i = 0; i < 6; i++) {
        occ = occ | this->bitboards[player][i];
    }
    return occ;
}


/*
 * Initializes the randomized Zobrist hash values array.
 * The Zobrist array should be initialized at the creation of a new Position.
 */
void init_zobrist() {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist;
    for (int i = 0; i < 781; i++) {
        _ZOBRIST_ARR[i] = dist(rng);
    }

}


/*
 * Gets the Zobrist hash of this position in the form of a 64-bit integer.
 */
uint64_t Position::zobrist() {
    uint64_t z = 0ULL;

    // XOR the values for each piece on each square of the board
    for (int i = 0; i < 12; i++) {
        uint64_t bb = this->bitboards[i / 6][i % 6];
        while (bb) {
            int sq = pop_lsb(bb); 
            z ^= _ZOBRIST_ARR[i + sq * 12];
        }
    }

    // XOR the values for each castling right
    int wcr = this->castling_rights[Player::WHITE];
    int bcr = this->castling_rights[Player::BLACK];

    if (wcr & CastlingRights::KING_UNMOVED) {
        if (wcr & CastlingRights::KROOK) z ^= _ZOBRIST_ARR[768];
        if (wcr & CastlingRights::QROOK) z ^= _ZOBRIST_ARR[769];
    }

    if (bcr & CastlingRights::KING_UNMOVED) {
        if (bcr & CastlingRights::KROOK) z ^= _ZOBRIST_ARR[770];
        if (bcr & CastlingRights::QROOK) z ^= _ZOBRIST_ARR[771];
    }

    // Include the EP target in the hash
    if (this->ep_target != -1) {
        int ep_file = this->ep_target % 8;

        if ((ep_file > 0 && this->bitboards[this->player_to_move][Piece::PAWN] & (1ULL << (this->ep_target - 1))) ||
            (ep_file < 7 && this->bitboards[this->player_to_move][Piece::PAWN] & (1ULL << (this->ep_target + 1)))) {
            z ^= _ZOBRIST_ARR[772 + ep_file];
        }
    }
    
    // Include the player to move in the hash
    if (this->player_to_move == Player::BLACK) z ^= _ZOBRIST_ARR[780];

    return z;
}
