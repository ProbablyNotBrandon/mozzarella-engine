#ifndef CASTLING_RIGHTS_H
#define CASTLING_RIGHTS_H

#include <cstdint>

enum CastlingRights: uint8_t {
    KING_UNMOVED  = 1 << 0,
    QROOK = 1 << 1,
    KROOK = 1 << 2
};

#endif
