#ifndef SLIDING_BLOOM_H
#define SLIDING_BLOOM_H

#include "bitset.h"
#include "hash.h"
#include "../definition.h"

class Sliding_Bloom {
private:
    const uint HASH_NUM;
    const uint STAGE_BIT;
    const uint LENGTH_BIT;
    const uint LO;
    double step;
    uint Clock_Pos;
    uint Last_Time;
    BitSet *future;
    BOBHash *hash;

    void Clock_Go(ulong num); //clock moves around the Sl-BF

public:
    Sliding_Bloom(uint _STAGE_BYTE, uint _HASH_NUM, uint _lo);

    ~Sliding_Bloom();

    void Init(const char *key, ulong time); //Insert data into Sl-BF

    bool Query(const char *key); //whether data is in the set
};


Sliding_Bloom::Sliding_Bloom(uint _STAGE_BYTE, uint _HASH_NUM, uint _lo) :
        HASH_NUM(_HASH_NUM), LO(_lo),
        STAGE_BIT(8 * _STAGE_BYTE),
        LENGTH_BIT(8 * _STAGE_BYTE * _HASH_NUM) {
    step = 1.0 * (HASH_NUM - LO) * STAGE_BIT / CYCLE;
    Clock_Pos = 0;
    Last_Time = 0;
    future = new BitSet(_STAGE_BYTE * HASH_NUM);
    hash = new BOBHash[HASH_NUM];
    for (int i = 0; i < HASH_NUM; ++i) {
        hash[i].initialize(i * 97);
    }
}

Sliding_Bloom::~Sliding_Bloom() {
    delete future;
    delete[] hash;
}

void Sliding_Bloom::Init(const char *key, ulong time) {
    Clock_Go(time * step);
    for (uint i = 0; i < HASH_NUM; ++i) {
        uint position = hash[i].run(key, KEY_LEN) % STAGE_BIT + i * STAGE_BIT;
        future->Set(position);
    }
}

bool Sliding_Bloom::Query(const char *key) {
    uint l_max = (Clock_Pos / STAGE_BIT + 1) % HASH_NUM;
    uint pointer_offset = Clock_Pos % STAGE_BIT;
    for (int i = 0; i < LO; ++i) {
        uint query_level = (l_max + i) % HASH_NUM;
        uint hash_offset = hash[query_level].run(key, KEY_LEN) % STAGE_BIT;
        uint hash_position = hash_offset + query_level * STAGE_BIT;

//        if (i < LO - 1 || i == LO - 1 && hash_offset < pointer_offset) {
        if (!future->Get(hash_position)) {
            return false;
        }
//        }

    }
    return true;
}

void Sliding_Bloom::Clock_Go(ulong time) {
    for (; Last_Time < time; ++Last_Time) {
        future->Clear(Clock_Pos);
        Clock_Pos = (Clock_Pos + 1) % LENGTH_BIT;
    }
}

#endif // SLIDING_BLOOM_H

