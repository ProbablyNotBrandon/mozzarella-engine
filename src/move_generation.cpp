#include "move_generation.h"

uint64_t PAWN_ADVANCE_MASKS[2][64] = {
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x1010000, 0x2020000, 0x4040000, 0x8080000, 0x10100000, 0x20200000, 0x40400000, 0x80800000 ,
        0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 ,
        0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000 ,
        0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000, 0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000 ,
        0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000, 0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
        0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    },
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
        0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
        0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
        0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
        0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
        0x10100000000, 0x20200000000, 0x40400000000, 0x80800000000, 0x101000000000, 0x202000000000, 0x404000000000, 0x808000000000,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    }
};
uint64_t PAWN_ATTACK_MASKS[2][64] = {
    {
        0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
        0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
        0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
        0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
        0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
        0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
        0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000, 0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    },
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x2, 0x5, 0xa, 0x14, 0x28, 0x50, 0xa0, 0x40,
        0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
        0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
        0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
        0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
        0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
        0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000
    }
};
uint64_t KNIGHT_MOVE_MASKS[64] = {
    0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000,
    0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010, 0x40200020,
    0x204000402, 0x508000805, 0xa1100110a, 0x1422002214, 0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040,
    0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000,
    0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000,
    0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
    0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
    0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000
};
uint64_t KING_MOVE_MASKS[64] = {
    0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040,
    0x30203, 0x70507, 0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0,
    0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000,
    0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000,
    0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000,
    0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000,
    0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
    0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000
};

uint64_t W_QS_CASTLE_MASK = 0xE;
uint64_t W_KS_CASTLE_MASK = 0x60;
uint64_t B_QS_CASTLE_MASK = 0xE00000000000000;
uint64_t B_KS_CASTLE_MASK = 0x6000000000000000;

std::vector<uint32_t> generate_legal_moves(Position *p) {
    // TODO: rework this to use an array pointer passed in to the function or something
    // There is no reason that this needs to use a whole std::vector and return a copy
    // of the vector every time it returns. 

    // Also, if you do rework this, you can use a C-style array and have each function
    // return a pointer to the end of the array OR just an integer representing how many
    // moves were added.

    std::vector<uint32_t> legal_moves;
    Player player = p->player_to_move;

    std::vector<uint32_t> (*generators[])(Position*) = {
        generate_pawn_moves,
        generate_en_passant_moves,
        generate_knight_moves,
        generate_bishop_moves,
        generate_rook_moves,
        generate_queen_moves//,
        //generate_king_moves,
        //generate_castle_moves
    };

    // There could be an optimization here considering that there can only be 2 checking
    // pieces at any given time.

    for (auto gen : generators) {
        for (uint32_t m : gen(p)) {
            p->move(m);
            if (!is_in_check(p, player)) legal_moves.push_back(m);
            p->unmove(m);
        }
    }

    if (!is_in_check(p, player)) {
        for (uint32_t m : generate_castle_moves(p)) {
            p->move(m);
            if (!is_in_check(p, player)) legal_moves.push_back(m);
            p->unmove(m);
        }
    }

    for (uint32_t m : generate_king_moves(p)) {
        p->move(m);
        if (!is_in_check(p, player)) legal_moves.push_back(m);
        p->unmove(m);
    }

    return legal_moves;
}

std::vector<uint32_t> generate_castle_moves(Position *p) {
    uint64_t all_occupied = p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK);
    
    std::vector<uint32_t> moves;
    
    // TODO: merge these shifts since they are the same, just different directions
    if (p->castling_rights[p->player_to_move] & CastlingRights::KING_UNMOVED) {
        if (p->castling_rights[p->player_to_move] & CastlingRights::QROOK) {
            bool castle_through_check = false;
            p->bitboards[p->player_to_move][Piece::KING] >>= 1;
            if (is_in_check(p, p->player_to_move)) castle_through_check = true;
            p->bitboards[p->player_to_move][Piece::KING] <<= 1;

            if (!castle_through_check) {
                if (p->player_to_move == Player::WHITE && !(W_QS_CASTLE_MASK & all_occupied)) {
                        moves.push_back(encode_move(4, 2, Piece::KING, 0, 0, MoveFlags::QUEEN_CASTLE));
                } else if (p->player_to_move == Player::BLACK && !(B_QS_CASTLE_MASK & all_occupied)) {
                        moves.push_back(encode_move(60, 58, Piece::KING, 0, 0, MoveFlags::QUEEN_CASTLE));
                }
            }
        }

        if (p->castling_rights[p->player_to_move] & CastlingRights::KROOK) {
            bool castle_through_check = false;
            p->bitboards[p->player_to_move][Piece::KING] <<= 1;
            if (is_in_check(p, p->player_to_move)) castle_through_check = true;
            p->bitboards[p->player_to_move][Piece::KING] >>= 1;

            if (!castle_through_check) {
                if (p->player_to_move == Player::WHITE && !(W_KS_CASTLE_MASK & all_occupied)) {
                    moves.push_back(encode_move(4, 6, Piece::KING, 0, 0, KING_CASTLE));
                } else if (p->player_to_move == Player::BLACK && !(B_KS_CASTLE_MASK & all_occupied)) {
                    moves.push_back(encode_move(60, 62, Piece::KING, 0, 0, KING_CASTLE));
                }
            }
        }
    }

    return moves;
}

std::vector<uint32_t> generate_en_passant_moves(Position *p) {
    std::vector<uint32_t> moves;

    if (!(p->move_stack.empty()) && get_flags(p->move_stack.back()) == MoveFlags::DOUBLE_PAWN_PUSH) {
        uint64_t occupied = (p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK));
        
        int push_to_sq = get_to_sq(p->move_stack.back());
        int to_sq = push_to_sq + (1 - 2 * (p->player_to_move)) * 8;
        int double_push_file = push_to_sq % 8;

        if (!(1ULL << to_sq & occupied)) {
            if (p->bitboards[p->player_to_move][Piece::PAWN] & (1ULL << (push_to_sq - 1)) && double_push_file != 0) {
                moves.push_back(encode_move(push_to_sq - 1, to_sq, Piece::PAWN, Piece::PAWN, 0, MoveFlags::EN_PASSANT | MoveFlags::CAPTURE));
            }

            if (p->bitboards[p->player_to_move][Piece::PAWN] & (1ULL << (push_to_sq + 1)) && double_push_file != 7) {
                moves.push_back(encode_move(push_to_sq + 1, to_sq, Piece::PAWN, Piece::PAWN, 0, MoveFlags::EN_PASSANT | MoveFlags::CAPTURE));
            }
        }
    }

    return moves;
}

std::vector<uint32_t> generate_pawn_moves(Position *p) {
    Player player = p->player_to_move;
    Player opponent = Player (1 - player);

    uint64_t occupied = p->get_occupied(player);
    uint64_t opponent_occupied = p->get_occupied(opponent);
    uint64_t all_occupied = occupied | opponent_occupied;
    uint64_t pawn_bb = p->bitboards[p->player_to_move][Piece::PAWN];

    std::vector<uint32_t> moves;

    while (pawn_bb) {
        int pawn_sq = pop_lsb(pawn_bb);

        uint64_t all_pawn_advances = PAWN_ADVANCE_MASKS[player][pawn_sq] & ~(all_occupied);

        // Edge case: a pawn cannot advance two steps if obstructed
        // TODO: change this to use bitboards and shifting directly (there is an (incomplete) Python implementation of this)
        if (player == Player::WHITE && 8 <= pawn_sq && pawn_sq <= 15) {
            if (all_occupied & (1ULL << (pawn_sq + 8))) {
                all_pawn_advances &= ~(1ULL << (pawn_sq + 16));
            }
        } else if (player == Player::BLACK && 48 <= pawn_sq && pawn_sq <= 55) {
            if (all_occupied & (1ULL << (pawn_sq - 8))) {
                all_pawn_advances &= ~(1ULL << (pawn_sq - 16));
            }
        }

        // This is kind of ridiculous.
        // Why am I iterating over a mask which will (83% of the time) have one item?
        while (all_pawn_advances) {
            int to_sq = pop_lsb(all_pawn_advances);

            uint32_t flags = 0;

            // If the target square is a promotion
            if ((56 * (opponent)) <= to_sq && to_sq <= (7 + 56 * (opponent))) {
                moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::QUEEN, MoveFlags::QUEEN_PROMO));
                moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::ROOK, MoveFlags::ROOK_PROMO));
                moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::BISHOP, MoveFlags::BISHOP_PROMO));
                moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::KNIGHT, MoveFlags::KNIGHT_PROMO));
                break;
            }
            if (std::abs(pawn_sq - to_sq) == 16) {
                flags = DOUBLE_PAWN_PUSH;
            }
            moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, 0, 0, flags));
        }

        uint64_t all_pawn_attacks = PAWN_ATTACK_MASKS[player][pawn_sq] & opponent_occupied;

        Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        while (all_pawn_attacks) {
            int to_sq = pop_lsb(all_pawn_attacks);

            for (Piece attackable_piece: attackable_pieces) {
                if (p->bitboards[opponent][attackable_piece] & (1ULL << to_sq)) {
                    if ((56 * opponent) <= to_sq && to_sq <= (7 + 56 * opponent)) {
                        moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::QUEEN, MoveFlags::QUEEN_PROMO | MoveFlags::CAPTURE));
                        moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::ROOK, MoveFlags::ROOK_PROMO | MoveFlags::CAPTURE));
                        moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::BISHOP, MoveFlags::BISHOP_PROMO | MoveFlags::CAPTURE));
                        moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::KNIGHT, MoveFlags::KNIGHT_PROMO | MoveFlags::CAPTURE));
                        break;
                    }
                    moves.push_back(encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, 0, MoveFlags::CAPTURE));
                }
            }
        }
    }

    return moves;
}

std::vector<uint32_t> generate_knight_moves(Position *p) {
    Player player = p->player_to_move;
    Player opponent = (Player) (1 - p->player_to_move);

    uint64_t occupied = p->get_occupied(player);
    uint64_t opponent_occupied = p->get_occupied(opponent);

    std::vector<uint32_t> moves;

    uint64_t knight_bb = p->bitboards[player][Piece::KNIGHT];

    Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};

    while (knight_bb) {
        int knight_sq = pop_lsb(knight_bb);

        uint64_t all_knight_moves = KNIGHT_MOVE_MASKS[knight_sq] & ~occupied;

        while (all_knight_moves) {
            int to_sq = pop_lsb(all_knight_moves);
            uint64_t to_bb = (1ULL << to_sq); 

            if (opponent_occupied & to_bb) {
                for (Piece attackable_piece: attackable_pieces) {
                    if (p->bitboards[opponent][attackable_piece] & to_bb) {
                        moves.push_back(encode_move(knight_sq, to_sq, Piece::KNIGHT, attackable_piece, 0, MoveFlags::CAPTURE));
                    }
                }
            } else {
                moves.push_back(encode_move(knight_sq, to_sq, Piece::KNIGHT, 0, 0, 0));
            }
        }
    }

    return moves;
}

std::vector<uint32_t> generate_king_moves(Position *p) {

    Player opponent = (Player) (1 - p->player_to_move);
    uint64_t king_bb = p->bitboards[p->player_to_move][Piece::KING];

    int king_sq = pop_lsb(king_bb);

    std::vector<int> to_sqs;

    uint64_t king_move_bb = KING_MOVE_MASKS[king_sq] & ~(p->get_occupied(p->player_to_move));

    uint64_t non_captures = king_move_bb & ~(p->get_occupied(opponent));
    uint64_t captures = KING_MOVE_MASKS[king_sq] & p->get_occupied(opponent);
    
    std::vector<uint32_t> moves;

    while (non_captures) {
        int to_sq = pop_lsb(non_captures);
        moves.push_back(encode_move(king_sq, to_sq, Piece::KING, 0, 0, 0));
    }

    Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
    while (captures) {
        int to_sq = pop_lsb(captures);
        uint64_t to_bb = (1ULL << to_sq);

        for (Piece attackable_piece: attackable_pieces) {
            if (to_bb & p->bitboards[opponent][attackable_piece]) {
                moves.push_back(encode_move(king_sq, to_sq, Piece::KING, attackable_piece, 0, MoveFlags::CAPTURE));
            }
        } 
    }

    return moves;
}

std::vector<uint32_t> generate_bishop_moves(Position *p) {
    int deltas[] = {-9, -7, 7, 9};
    return generate_sliding_moves(p, Piece::BISHOP, deltas, 4);
}

std::vector<uint32_t> generate_rook_moves(Position *p) {
    int deltas[] = {-8, -1, 1, 8};
    return generate_sliding_moves(p, Piece::ROOK, deltas, 4);
}

std::vector<uint32_t> generate_queen_moves(Position *p) {
    int deltas[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    return generate_sliding_moves(p, Piece::QUEEN, deltas, 8);
}

std::vector<uint32_t> generate_sliding_moves(Position *p, Piece piece, int deltas[], int ndeltas) {
    Player player = p->player_to_move;
    Player opponent = (Player) (1 - player);

    uint64_t occupied = p->get_occupied(player);
    uint64_t opponent_occupied = p->get_occupied(opponent);

    std::vector<uint32_t> moves;

    Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
    
    uint64_t piece_sqs = p->bitboards[player][piece];

    while (piece_sqs) {
        int piece_sq = pop_lsb(piece_sqs);
        
        int piece_file = piece_sq % 8;

        for (int i = 0; i < ndeltas; i++) {
            int last_file = piece_file;

            for (int step = 1; step < 8; step++) {
                int to_sq = piece_sq + deltas[i] * step;
                uint64_t to_bb = (1ULL << to_sq);
                int to_file = to_sq % 8;

                if ((to_sq < 0 || to_sq > 63)
                    || (abs(to_file - last_file) > 1)
                    || (occupied & (1ULL << to_sq))) {
                    break;
                }

                if (opponent_occupied & to_bb) {
                    for (Piece attackable_piece: attackable_pieces) {
                        if (p->bitboards[opponent][attackable_piece] & to_bb) {
                            moves.push_back(encode_move(piece_sq, to_sq, piece, attackable_piece, 0, MoveFlags::CAPTURE));
                            break;
                        }
                    }
                    break;
                } else {
                    moves.push_back(encode_move(piece_sq, to_sq, piece, 0, 0, 0));
                }

                last_file = to_file;
            }
        }
    }

    return moves;
}

bool is_in_check(Position *p, Player player) {
    Player opponent = (Player) (1 - player);
    uint64_t king_bb = p->bitboards[player][Piece::KING];
    int king_sq = pop_lsb(king_bb);

    if (KNIGHT_MOVE_MASKS[king_sq] & p->bitboards[opponent][Piece::KNIGHT]) {
        return true;
    }
    if (PAWN_ATTACK_MASKS[player][king_sq] & p->bitboards[opponent][Piece::PAWN]) {
        return true;
    }
    if (KING_MOVE_MASKS[king_sq] & p->bitboards[opponent][Piece::KING]) {
        return true;
    }

    return is_in_sliding_check(p, player);
}

bool is_in_sliding_check(Position* p, Player player) {
    // Locate the king square
    uint64_t king_bb = p->bitboards[player][Piece::KING];
    int king_sq = pop_lsb(king_bb);
    int king_file = king_sq % 8;
    Player opponent = (Player) (1 - player);

    // Opponent's sliding piece bitboards
    uint64_t opp_bishops_and_queens = p->bitboards[opponent][Piece::BISHOP] | p->bitboards[opponent][Piece::QUEEN];
    uint64_t opp_rooks_and_queens   = p->bitboards[opponent][Piece::ROOK] | p->bitboards[opponent][Piece::QUEEN];

    uint64_t all_occupied = (p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK));

    // Sliding directions
    uint64_t sliding_bbs[] = {opp_bishops_and_queens, opp_rooks_and_queens};
    int deltas[2][4] = {{-9, -7, 7, 9}, {-8, -1, 1, 8}};

    // Loop over bishop- and rook-like attackers
    for (int i = 0; i < 2; i++) {
        uint64_t occupied_no_sliding = (all_occupied & ~(sliding_bbs[i]));
        for (int j = 0; j < 4; j++) {
            int curr_file = king_file;

            for (int step = 1; step < 8; step++) {
                int to_sq = king_sq + deltas[i][j] * step;
                uint64_t to_bb = (1ULL << to_sq);
                int to_file = to_sq % 8;

                if (to_sq < 0 || to_sq > 63) break;
                if (abs(to_file - curr_file) > 1) break;
                if (occupied_no_sliding & to_bb) break;

                if (sliding_bbs[i] & to_bb) {
                    return true;
                }

                curr_file = to_file;
            }
        }
    }

    return false;
}

void render_bitboard(uint64_t bb, int origin_sq = -1, char symbol = 'x') {
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));

    for (int i = 0; i < 64; i++) {
        if ((bb >> i) & 1ULL) {
            int row = 7 - (i / 8);
            int col = i % 8;
            board[row][col] = symbol;
        }
    }

    if (origin_sq != -1) {
        int row = 7 - (origin_sq / 8);
        int col = origin_sq % 8;
        board[row][col] = 'P';
    }

    for (const auto& row : board) {
        for (char c : row) {
            std::cout << c << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
