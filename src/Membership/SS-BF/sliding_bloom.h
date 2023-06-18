#ifndef SLIDING_BLOOM_H
#define SLIDING_BLOOM_H

#include "bitset.h"
#include "hash.h"
#include "../definition.h"

class Sliding_Bloom {
private:
    const uint STAGE_BYTE;
    const uint HASH_NUM;
    const uint LENGTH_BYTE;
    const uint STAGE_BIT;
    const uint LENGTH_BIT;
    uint Clock_Pos;
    uint Last_Time;
    BitSet *future;
    BitSet *now;
    BOBHash *hash;

    void Clock_Go(ulong num); //clock moves around the Sl-BF

public:
    Sliding_Bloom(uint _STAGE_BYTE, uint _HASH_NUM);

    ~Sliding_Bloom();

    void Init(const char *key, ulong time); //Insert data into Sl-BF

    bool Query(const char *key); //whether data is in the set
};


Sliding_Bloom::Sliding_Bloom(uint _STAGE_BYTE, uint _HASH_NUM) :
        STAGE_BYTE(_STAGE_BYTE), HASH_NUM(_HASH_NUM), LENGTH_BYTE(_STAGE_BYTE * _HASH_NUM),
        STAGE_BIT(8 * _STAGE_BYTE), LENGTH_BIT(8 * _STAGE_BYTE * _HASH_NUM) {
    Clock_Pos = 0;
    Last_Time = 0;
    future = new BitSet(LENGTH_BYTE);
    now = new BitSet(LENGTH_BYTE);
    hash = new BOBHash[HASH_NUM];
    for (int i = 0; i < HASH_NUM; ++i) {
        hash[i].initialize(i * 97);
    }
}

Sliding_Bloom::~Sliding_Bloom() {
    delete future;
    delete now;
    delete[] hash;
}

void Sliding_Bloom::Init(const char *key, ulong time) {
    Clock_Go(time * (LENGTH_BIT / CYCLE));
    for (uint i = 0; i < HASH_NUM; ++i) {
        uint position = hash[i].run(key, KEY_LEN) % STAGE_BIT + i * STAGE_BIT;
        future->Set(position);
    }
}

bool Sliding_Bloom::Query(const char *key) {
    for (uint i = 0; i < HASH_NUM; ++i) {
        uint position = hash[i].run(key, KEY_LEN) % STAGE_BIT + i * STAGE_BIT;
        if (!future->Get(position) && !now->Get(position))
            return false;
    }
    return true;
}

void Sliding_Bloom::Clock_Go(ulong num) {
    for (; Last_Time < num; ++Last_Time) {
        now->Clear(Clock_Pos);
        if (future->Get(Clock_Pos)) {
            now->Set(Clock_Pos);
            future->Clear(Clock_Pos);
        }
        Clock_Pos = (Clock_Pos + 1) % LENGTH_BIT;
    }
}


#endif // SLIDING_BLOOM_H

