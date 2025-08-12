#include "evaluation.h"

int evaluate(Position *p, Player player) {
    return p->material_value[player] - p->material_value[1 - player]; 
}
