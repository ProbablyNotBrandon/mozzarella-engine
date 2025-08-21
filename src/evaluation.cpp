#include "evaluation.h"

int evaluate(Position *p) {
    return p->material_value[Player::WHITE] - p->material_value[Player::BLACK]; 
}
