#ifndef __TT_H__
#define __TT_H__

#include <cstdint>
#include <cstddef>

enum class Bound { EXACT, LOWER, UPPER };

typedef struct TTEntry {
    uint64_t key;
    int depth;
    int score;
    Bound flag;
} TTEntry;

class TranspositionTable {
public:
    TranspositionTable(size_t mb = 64);
    ~TranspositionTable();

    bool probe(uint64_t key, int depth, int *score, int alpha, int beta);
    void store(uint64_t key, int depth, int score, int alpha, int beta);
    void clear();

    size_t size;
    int occupancy;
    TTEntry *table;
};

#endif
