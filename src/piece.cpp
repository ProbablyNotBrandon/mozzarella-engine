#include "piece.h"

int piece_value(Piece piece) {
    static const int values[6] = {100, 320, 330, 500, 900, 0};
    return values[piece];
}
