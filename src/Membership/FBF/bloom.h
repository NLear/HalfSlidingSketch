#ifndef BLOOM_H
#define BLOOM_H

/*
 * Bloom Filter
 */

#include "../definition.h"

class Bloom {
private:
    const uint HASH_NUM;
    const uint LENGTH;
    BitSet *bitset;
    BOBHash *hash;

public:
    Bloom(uint _LENGTH, uint _HASH_NUM);

    ~Bloom();

    void Init(const char *key); //Insert data into bloom filter

    bool Query(const char *key); //whether data is in the set
};

Bloom::Bloom(uint _LENGTH, uint _HASH_NUM) : LENGTH(_LENGTH), HASH_NUM(_HASH_NUM) {
    bitset = new BitSet(LENGTH);
    hash = new BOBHash[HASH_NUM];
    for (int i = 0; i < HASH_NUM; ++i) {
        hash[i].initialize(i * 97);
    }
}

Bloom::~Bloom() {
    delete bitset;
    delete [] hash;
}

void Bloom::Init(const char *key) {
    for (uint i = 0; i < HASH_NUM; ++i) {
        uint position = hash[i].run(key, KEY_LEN) % LENGTH;
        bitset->Set(position);
    }
}

bool Bloom::Query(const char *key) {
    for (uint i = 0; i < HASH_NUM; ++i) {
        uint position = hash[i].run(key, KEY_LEN) % LENGTH;
        if (!bitset->Get(position))
            return false;
    }
    return true;
}

#endif // BLOOM_H
