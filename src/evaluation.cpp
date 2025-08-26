#include "evaluation.h"

// Returns a static evaluation for white
int evaluate(Position *p) {
    // return p->material_value[Player::WHITE] - p->material_value[Player::BLACK];
    return p->pst_eval + p->material_value[Player::WHITE] - p->material_value[Player::BLACK];
}
