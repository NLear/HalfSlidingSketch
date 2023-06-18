#ifndef BITSET_H
#define BITSET_H

/*
 * My bitset
 */

#include <string.h>

class BitSet
{
public:
    BitSet(uint _LENGTH);
    ~BitSet();
    void Set(uint index); //set the indexed bit to 1
    void Clear(uint index); //set the indexed bit to 0
    bool Get(uint index); //return the indexed bit

private:
    unsigned char * bitset;
};


BitSet::BitSet(uint _LENGTH){
    uint size = (_LENGTH >> 3) + 1;
    bitset = new unsigned char[size];
    memset(bitset, 0, size * sizeof(unsigned char));
}

BitSet::~BitSet(){
    delete [] bitset;
}

void BitSet::Set(uint index){
    uint position = (index >> 3);
    uint offset = (index & 0x7);
    bitset[position] |= (1 << offset);
}

void BitSet::Clear(uint index){
    uint position = (index >> 3);
    uint offset = (index & 0x7);
    unsigned char mask = 0xff;
    mask ^= (1 << offset);
    bitset[position] &= mask;
}

bool BitSet::Get(uint index){
    uint position = (index >> 3);
    uint offset = (index & 0x7);
    if((bitset[position] & (1 << offset)) != 0)
        return true;
    return false;
}


#endif // BITSET_H
