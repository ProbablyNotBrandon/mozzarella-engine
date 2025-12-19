#include <string.h>

#include "tt.h"


/*
 * Constructor for the TranspositionTable.
 * mb is the allowed size of the transposition table in megabytes (MB).
 */
TranspositionTable::TranspositionTable(size_t mb) {
    this->size = (mb * 1024 * 1024) / sizeof(TTEntry);
    this->table = new TTEntry[size];
    this->clear();
}


/*
 * Destructor for the TranspositionTable.
 */
TranspositionTable::~TranspositionTable() {
    delete[] this->table;
}

/*
 * Probes the transposition table for the given Zobrist key.
 * Returns true if a usable entry exists for the current depth and alpha-beta window.
 */
bool TranspositionTable::probe(uint64_t key, int depth, int alpha, int beta) {
    TTEntry entry = table[key % this->size];

    if (entry.key != key || entry.depth < depth) return false;

    if (entry.flag == Bound::EXACT) return true;
    else if (entry.flag == Bound::LOWER && entry.score >= beta) return true;
    else if (entry.flag == Bound::UPPER && entry.score <= alpha) return true;

    return false;
}


/*
 * Stores a position in the transposition table.
 * Determines the bound (EXACT / LOWER / UPPER) based on alpha, beta, and score.
 */
void TranspositionTable::store(uint64_t key, int depth, int score, int alpha, int beta) {
    TTEntry entry = table[key % this->size];

    if (entry.key == 0) this->occupancy++;

    entry.key = key;
    entry.depth = depth;
    entry.score = score;

    if (score <= alpha) entry.flag = Bound::UPPER;
    else if (score >= beta) entry.flag = Bound::LOWER;
    else entry.flag = Bound::EXACT;
}


/*
 * Zeroes out the transposition table and reset occupancy count.
 */
void TranspositionTable::clear() {
    memset(this->table, 0, (this->size * sizeof(TTEntry)));
    this->occupancy = 0;
}
