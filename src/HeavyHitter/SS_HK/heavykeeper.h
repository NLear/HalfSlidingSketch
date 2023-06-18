#ifndef _heavykeeper_H
#define _heavykeeper_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "BOBHash32.h"
#include "../definition.h"
#include "BOBHash64.h"

#define L 10
#define HK_b 1.08

class heavykeeper {
public:
    struct node {
        string FP;
        int *C;
    } HK[L][MAX_MEM + 10];
    BOBHash64 *bobhash;
    int B; //每一级的深度
    int cycle;
    double time_now;
    int time_pos;
    int field_num;

    heavykeeper(int M2, int cycle, int field_num) : B(M2), cycle(cycle), field_num(field_num) {
        //bobhash=new BOBHash64(1005);time_now=0;time_pos=0;
        for (int hk_i = 0; hk_i < L; hk_i++) {
            for (int hk_j = 0; hk_j < MAX_MEM + 10; hk_j++) {
                HK[hk_i][hk_j].C = new int[field_num];
                memset(HK[hk_i][hk_j].C, 0, field_num * sizeof(int));
            }
        }
    }

    ~heavykeeper() {
        for (int hk_i = 0; hk_i < L; hk_i++) {
            for (int hk_j = 0; hk_j < MAX_MEM + 10; hk_j++) {
                delete[] HK[hk_i][hk_j].C;
            }
        }
    }

    void clear() {
        for (int i = 0; i < L; i++)
            for (int j = 0; j <= B + 5; j++)
                for (int k = 0; k < field_num; k++) {
                    HK[i][j].C[k] = 0;
                    HK[i][j].FP.clear();
                }
    }

    //hash function
    unsigned long long Hash(int j, string ST) {
        bobhash = new BOBHash64(j + 10);
        unsigned long long ans = (bobhash->run(ST.c_str(), ST.size()));
        delete bobhash;

        return ans;

    }

    //update an item
    void Insert(string x, int time_stamp) {
        //用一层
        if (time_stamp > (cycle / (field_num))) {
            int time_i = 0;
            for (; ((time_i + 1) * cycle / field_num / L / B) + time_now < time_stamp; time_i++) {
                int hki = time_pos / B;
                int hkj = time_pos % B;

                for (int nk = field_num - 1; nk > 0; nk--) {
                    HK[hki][hkj].C[nk] = HK[hki][hkj].C[nk - 1];
                }
                HK[hki][hkj].C[0] = 0;
                time_pos++;
                time_pos = time_pos % (B * L);
            }
            time_now = time_now + ((time_i + 0.0) * cycle / field_num / L / B);

        } else {
            time_now = time_stamp;
        }

        //calculate finger print

        //int FP=(H>>48);
        //L hash
        for (int j = 0; j < L; j++) {
            //calculate position
            unsigned long long H = Hash(j, x);
            int Hsh = H % (B - (2 * L) + 2 * j + 3);
            //If the finger print indicates that it is the same element
            if (HK[j][Hsh].FP == x) {
                HK[j][Hsh].C[0]++;
            } else { //Not the same element
                int prob = 0;
                for (int prob_i = 0; prob_i < field_num; prob_i++) {
                    prob = prob + HK[j][Hsh].C[prob_i];
                }
                //probability decay
                if (!(rand() % int(pow(HK_b, prob)))) {
                    bool flag = false;
                    for (int decay_i = 0; decay_i < field_num; decay_i++) {
                        if (HK[j][Hsh].C[decay_i] != 0) {
                            HK[j][Hsh].C[decay_i]--;
                            flag = true;
                            break;
                        }
                    }
                    if (flag == false) {
                        HK[j][Hsh].FP = x;
                        HK[j][Hsh].C[0] = 1;
                    }
                }
            }
        }
    }

    //query an item
    int num_query(string x) {

        //int FP=(H>>48);
        int maxv = 0;
        for (int j = 0; j < L; j++) {
            unsigned long long H = Hash(j, x);
            int Hsh = H % (B - (2 * L) + 2 * j + 3);
            if (HK[j][Hsh].FP == x) {
                int prob = 0;
                for (int prob_i = 0; prob_i < field_num; prob_i++) {
                    prob = prob + HK[j][Hsh].C[prob_i];
                }
                maxv = max(maxv, prob);
            }
        }
        return maxv;
    }

};

#endif
