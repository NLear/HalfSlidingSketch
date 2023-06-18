#ifndef FORGET_BLOOM_H
#define FORGET_BLOOM_H

/*
 * FBF, Forgetful Bloom Filter
 */

#include "bitset.h"
#include "hash.h"
#include "bloom.h"
#include "../definition.h"

class Forget_Bloom {
private:

    const uint LENGTH;
    const uint HASH_NUM;
    const uint BLOOM_NUM; //how many bloom filter in the FBF
    uint Last_Time;
    uint future;  //which bloom filter is the "future"
    Bloom **bloom;

    void Update(ulong time); //clear the oldest bloom filter and new a bloom filter

public:
    Forget_Bloom(uint _LENGTH, uint _HASH_NUM, uint _BLOOM_NUM, uint _CYCLE);

    ~Forget_Bloom();

    void Init(const char *key, ulong time); //Insert data into FBF

    bool Query(const char *key); //whether data is in the set
};


Forget_Bloom::Forget_Bloom(uint _LENGTH, uint _HASH_NUM, uint _BLOOM_NUM, uint _CYCLE) :
        LENGTH(_LENGTH), HASH_NUM(_HASH_NUM), BLOOM_NUM(_BLOOM_NUM) {
    future = 0;
    Last_Time = 0;
    bloom = new Bloom *[BLOOM_NUM];
    for (uint i = 0; i < BLOOM_NUM; ++i) {
        bloom[i] = new Bloom(LENGTH, HASH_NUM);
    }
}

Forget_Bloom::~Forget_Bloom() {
    for (uint i = 0; i < BLOOM_NUM; ++i) {
        delete bloom[i];
    }
    delete[] bloom;
}

void Forget_Bloom::Init(const char *key, ulong time) {
    Update(time * (BLOOM_NUM - 1) / CYCLE);
    if (!this->Query(key)) {
        bloom[future]->Init(key);
        bloom[(future + 1) % BLOOM_NUM]->Init(key);
    }
}

bool Forget_Bloom::Query(const char *key) {
    if (bloom[future]->Query(key))
        return true;
    for (int i = (future + 1) % BLOOM_NUM; (i + 1) % BLOOM_NUM != future; i = (i + 1) % BLOOM_NUM) {
        if (bloom[i]->Query(key) && bloom[(i + 1) % BLOOM_NUM]->Query(key))
            return true;
    }
    if (bloom[(future + BLOOM_NUM - 1) % BLOOM_NUM]->Query(key))
        return true;
    return false;
}

void Forget_Bloom::Update(ulong time) {
    for (; Last_Time < time; ++Last_Time) {
        future = (future + 1) % BLOOM_NUM;
        delete bloom[future];
        bloom[future] = new Bloom(LENGTH, HASH_NUM);
    }
}


#endif // FORGET_BLOOM_H
