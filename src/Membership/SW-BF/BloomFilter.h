#include<cstring>
#include"BOBHash.h"
#include "../definition.h"

class BloomFilter {
private:
    int size = 0;
    int hash_num = 3;
    bool *bf;
    BOBHash *hash;
public:
    BloomFilter() {}

    BloomFilter(int _size, int _hash_num = 4) {
        size = _size;
        hash_num = _hash_num;
        bf = new bool[size];
        memset(bf, 0, sizeof(bool) * size);
        hash = new BOBHash[hash_num];
        for (int i = 0; i < hash_num; i++)
            hash[i].initialize(97 * i);
    }

    void init(int _size, int _hash_num = 4) {
        size = _size;
        hash_num = _hash_num;
        bf = new bool[size];
        memset(bf, 0, sizeof(bool) * size);
        hash = new BOBHash[hash_num];
        for (int i = 0; i < hash_num; i++)
            hash[i].initialize(97 * i);
    }

    void insert(const char *str) {
        for (int i = 0; i < hash_num; i++) {
            int temp = hash[i].run(str, KEY_LEN) % size;
            bf[temp] = true;
        }
    }

    bool query(const char *str) {
        bool flag = true;
        for (int i = 0; i < hash_num; i++) {
            int temp = hash[i].run(str, KEY_LEN) % size;
            flag &= bf[temp];
        }
        return flag;
    }

    void clear() {
        memset(bf, 0, sizeof(bool) * size);
    }

    int count() {
        int cnt = 0;
        for (int i = 0; i < size; i++)
            if (bf[i])
                cnt++;
        return cnt;
    }
};
