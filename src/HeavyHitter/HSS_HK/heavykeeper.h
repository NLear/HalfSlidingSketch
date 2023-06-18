#ifndef _heavykeeper_H
#define _heavykeeper_H

#include "hash_class.h"
#include "../definition.h"

#define LEVEL 7
#define L_O 1
#define HK_d 1
#define HK_b 1.08

struct node {
    string FP;
    int C;
};

class heavykeeper {
private:
    int level;
    int M2;
public:
    node **HK;//[HK_d][MAX_MEM + 10]

    heavykeeper() {}

    heavykeeper(int _level, int _M2) : level(_level), M2(_M2) {}

    ~heavykeeper() {
        for (int i = 0; i < HK_d; i++) {
            delete[] HK[i];
        }
        delete[] HK;
    }

    void init(int _level, int _M2) {
        level = _level;
        M2 = _M2;
        HK = new node *[HK_d];
        for (int i = 0; i < HK_d; i++) {
            HK[i] = new node[M2];
        }
        this->clear();
    }

    void clear() {
        for (int i = 0; i < HK_d; i++) {
            for (int j = 0; j < M2; j++) {
                HK[i][j].C = 0;
                HK[i][j].FP.clear();
            }
        }
    }

    void clear(int j) {
        for (int i = 0; i < HK_d; i++) {
            HK[i][j].C = 0;
            HK[i][j].FP.clear();
        }
    }

    void Insert(string x) {
        for (int j = 0; j < HK_d; j++) {
            unsigned int Hsh = Hash((unsigned char *) x.c_str(), (level * HK_d + j) % 13, DATA_LEN) % M2;
            if (HK[j][Hsh].FP == x) {
                HK[j][Hsh].C++;
            } else {
                if (!(rand() % int(pow(HK_b, HK[j][Hsh].C)))) {
                    HK[j][Hsh].C--;
                    if (HK[j][Hsh].C <= 0) {
                        HK[j][Hsh].FP = x;
                        HK[j][Hsh].C = 1;
                    }
                }
            }
        }
    }

    //query an item
    int Query(string x) {
        int max_v = 0;
        for (int j = 0; j < HK_d; j++) {
            unsigned int Hsh = Hash((unsigned char *) x.c_str(), (level * HK_d + j) % 13, DATA_LEN) % M2;
            if (HK[j][Hsh].FP == x) {
                max_v = max(max_v, HK[j][Hsh].C);
            }
        }
        return max_v;
    }

    map<string, int> GetHHs() {
        map<string, bool> vis;
        map<string, int> HK_HH;

        for (int i = 0; i < HK_d; i++) {
            for (int j = 0; j < M2; j++) {
                string key = HK[i][j].FP;
                if (vis.find(key) == vis.end()) {
                    vis[key] = true;
                    int est = this->Query(key);
                    if (est > PHI) {
                        HK_HH[key] = est;
                    }
                }
            }
        }
        return HK_HH;
    }
};

class HSS_HeavyKeeper {
private:
    int level, hk_M;
    int slots;
    double step, time_now;
    int time_pos;
    heavykeeper *hk;
public:
    HSS_HeavyKeeper(int _level, int _hk_M) : level(_level), hk_M(_hk_M), time_now(0), time_pos(0) {
        slots = _level * _hk_M;
        step = (double) CYCLE / ((_level - L_O) * _hk_M);
        hk = new heavykeeper[_level];
        for (int i = 0; i < _level; i++) {
            hk[i].init(i, _hk_M);
        }
    }

    ~HSS_HeavyKeeper() {
        delete[] hk;
    }

    void Insert(string key, long long int num) {
        for (; time_now + step < num; time_now += step) {
            int l = time_pos / hk_M % level;
            int offset = time_pos % hk_M;
            hk[l].clear(offset);
            time_pos++;
            time_pos %= slots;
        }

        for (int i = 0; i < level; i++) {
            hk[i].Insert(key);
        }
    }

    int Query(string key) {
        int ret = 0;
        for (int i = 0; i < level; i++) {
            int Hsh = Hash((unsigned char *) key.c_str(), i % 13, DATA_LEN) % hk_M;
            if (hk[i].HK[0][Hsh].FP == key) {
                ret = max(ret, hk[i].HK[0][Hsh].C);
            }
        }
        return ret;
    }

    map<string, int> GetHHs() {
        map<string, bool> vis;
        map<string, int> HK_HH;

        for (int l = 0; l < level; l++) {
            for (int i = 0; i < HK_d; i++) {
                for (int j = 0; j < hk_M; j++) {
                    string key = hk[l].HK[i][j].FP;
                    if (vis.find(key) == vis.end()) {
                        vis[key] = true;
                        int est = this->Query(key);
                        if (est > PHI) {
                            HK_HH[key] = est;
                        }
                    }
                }
            }
        }
        return HK_HH;
    }

};

#endif
