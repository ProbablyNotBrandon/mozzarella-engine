#include "uci.h"

uint32_t parse_move(Position pos, std::string move_str) {
    int from_sq = coord_to_sq(move_str.substr(0, 2));
    int to_sq = coord_to_sq(move_str.substr(2, 4));

    uint32_t flags = 0;

    // Get the promotion piece if it exists
    Piece promo_piece = Piece::PAWN;
    if (move_str.size() == 5) {
        char c = move_str[4];
        switch (c) {
            case 'q': 
                promo_piece = Piece::QUEEN;
                flags |= (MoveFlags::QUEEN_PROMO);
            case 'n': 
                promo_piece = Piece::KNIGHT;
                flags |= (MoveFlags::KNIGHT_PROMO);
            case 'b': 
                promo_piece = Piece::BISHOP;
                flags |= (MoveFlags::BISHOP_PROMO);
            case 'r': 
                promo_piece = Piece::ROOK;
                flags |= (MoveFlags::ROOK_PROMO);
        }
    }

    // Get the piece that was moved
    Piece piece_moved = Piece::PAWN;
    for (int i = 0; i < 6; i++) {
        if (pos.bitboards[pos.player_to_move][i] & (1ULL << from_sq)) {
            piece_moved = (Piece) i;
            break;
        }
    }

    // Check if the move was a castle (king moved two squares)
    if (piece_moved == Piece::KING) {
        if (from_sq + 2 == to_sq) {
            flags |= MoveFlags::KING_CASTLE;
        } else if (from_sq - 2 == to_sq) {
            flags |= MoveFlags::QUEEN_CASTLE;
        }
    }

    // Figure out if a piece has been captured
    int piece_captured = Piece::PAWN;
    for (int i = 0; i < 5; i++) {
        if (pos.bitboards[1 - pos.player_to_move][i] & (1ULL << to_sq)) {
            piece_captured = (Piece) i;
            flags |= MoveFlags::CAPTURE;
            break;
        }
    }

    if (!(flags & MoveFlags::CAPTURE) && piece_moved == Piece::PAWN) {
        for (auto res : {to_sq - 9, to_sq - 7, to_sq + 7, to_sq + 9}) {
            if (from_sq == res) {
                flags |= (MoveFlags::CAPTURE | MoveFlags::EN_PASSANT);
            }
        }
    }

    return encode_move(from_sq, to_sq, piece_moved, piece_captured, promo_piece, flags);
}
